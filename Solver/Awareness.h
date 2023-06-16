#pragma once
#include "ClawUnit.h"

ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
Timer nextionTimer(50);

CsT scannerCs;  //x-смотрит на куб, y-в сторону откидывания
class Scanner() {
public:
  Servo servo;
  uint8_t servoAngles[4];  //folded 0,1,2,3 centre
  void setServo(uint8_t angle) {
    servo.write(angle);
  }
  void setPosition(uint8_t index) {
    setServo(servoAngles[index]);
  }
  Scanner() {}
}
Scanner scanner();

CsT cubeCs;  //ориентация куба изначально не важна, закреплена за центральными клетками которые не считаются деталями

path::Branch solveTree[20];  //кодирование пути сборки

Cube::State currentState;  //current state
Cube::State newState;      //new state for fast checking

void opDemo() {
  Cube::resetRealPieces();
  currentState.updateCAI();
  while (true) {
    currentState.printSliced(false);
    Serial.print("Penalty:");
    Serial.println(currentState.statePenalty());
    // SCS::Space[i].basis.print();

    Serial.println();
    while (Serial.available() < 2) {}  //waiit for input
    uint8_t ov = Serial.read() - 48, oa = Serial.read() - 48;
    if (ov + oa == 18) break;
    Operation op(ov, oa);
    // i = SCS::getPostOpearationIndex(i, op);
    op.print();
    currentState.applyOperation(op);
  }
}

const uint8_t maxCommandLength = 5;
struct ValidBuffer {  //buffer for validated chars
  uint8_t length;
  uint8_t cursor = 0;
  char letters[maxCommandLength];
  uint8_t add(char value) {
    if (length < maxCommandLength) {
      letters[length] = value;
      length++;
    } else {
      Serial.println("ERR:buffer overflow!");
    }
    return (length);
  }
  void reset() {  //reset cursor to read buffer from first char next time
    cursor = 0;
  }
  void erase() {  //assume buffer is empty
    length = 0;
    cursor = 0;
  }
  uint8_t available() {  //number of characters available to read
    return (length - cursor);
  }
  char read() {  //
    if (cursor < length) {
      cursor++;
      return (letters[cursor - 1]);
    }
  }
  char peek() {
    if (cursor < length) {
      return (letters[cursor]);
    }
  }
  uint8_t readNumber() {
    if (cursor < length) {
      cursor++;
      return (letters[cursor - 1] - 48);
    }
  }
  uint8_t getNumber(uint8_t index) {
    return (letters[index] - 48);
  }
  void print() {
    Serial.print("l=");
    Serial.print(length);
    Serial.print("\t c=");
    Serial.print(cursor);
    Serial.print("\t");
    for (uint8_t i = 0; i < maxCommandLength; i++) {
      Serial.print(letters[i]);
    }
    sPnl();
  }
};  //
struct Port {
  uint8_t lastValidReciever = 0;
  long baudRate = 9600;
  bool tryConnect = true;
  bool connectionRequired = false;
  ValidBuffer buffer;
  HardwareSerial* port;
  Port(HardwareSerial* theport) {
    port = theport;
  }
  void begin() {
    port->begin(baudRate);
  }
};
class ICommandable {  //can validate that buffer contains valid command and do something
public:
  virtual uint8_t validateBuffer(ValidBuffer* buffer);  //0 not valid, 1 valid not complete, 2 valid and commited.
};
class IRstate : public ICommandable, public IUpdatable {  //class that can take commands, can operate all robot hardware on Update basis
};

IRstate* robotState;

/*About Serial connection. robotState can interprete custom commands specified for it recieved from interface device.
  PostMan class manages Serial communication between interface device ID, robot state RS and external device (PC)  
  PC<->PM(->RS->ID),(<->ID)
  RS sends commands on it's own and isn't being filtered                              RS->ID
  ID (Serial1) sends commands through PostMan, may be filtered or repeated to PC      ID->PM->RS,PC
  PC (Serial) sends commands to PostMan, may be repeated to RS or ID                  PC->PM->RS,ID
  
  RS has number of acceptable commands of specific length
  PM has number of specific commands that won't be retranslated if detected, used for debug from PC
  ID(Nextion) commands are send directly from RS or retranslated from PC via PM
  
  PostMan has compacted boolean variables [0:mute ID, 1:enable repeat, 2:repeat to 0=RS/1=ID]
  Again, postman recives it's commands only from PC and won't repeat them even when repat is available  */
void comEnd() {  //Nextion command rermination
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
}
class PostMan : public IRstate {
  bool repeatPC = true;           //repeat input from serial to Nextion or RS
  bool repeatPCtoNextion = true;  //true Nextion, false - RS
  bool repeatNextion = true;
  bool muteNextion = false;
  bool debug = false;

  Port ports[2] = { Port((HardwareSerial*)&Serial), Port((HardwareSerial*)&Serial1) };

  void end() {
    Serial.println(";");
  }
  void introduction() {
    Serial.print("PostMan: ");
  }
  void aboutPCcopy() {
    Serial.print("PC: ");
    if (repeatPC) {
      Serial.print("copy to ");
      if (repeatPCtoNextion) {
        Serial.print("Nextion");
      } else Serial.print("RState");
    } else Serial.print("don't copy");
  }
  void status() {
    introduction();
    Serial.println("status:");
    aboutPCcopy();
    end();
    Serial.print("Nextion: ");
    if (muteNextion) {
      Serial.print("muted");
      if (repeatNextion) {
        Serial.print(", ");
      }
    }
    if (repeatNextion) {
      Serial.print("copy to PC");
    } else if (!muteNextion) Serial.print("default");
    end();
  }
  void toggleNextionMute() {
    muteNextion = !muteNextion;
    introduction();
    Serial.print("Nextion ");
    if (!muteNextion) Serial.print("un");
    Serial.println("muted!");
  }
  void toggleNextionRepeat() {
    repeatNextion = !repeatNextion;
    introduction();
    if (repeatNextion) {
      Serial.print("copy");
    } else Serial.print("stopped copying");
    Serial.println(" Nextion;");
  }
  void togglePCRepeat() {
    repeatPC = !repeatPC;
    introduction();
    aboutPCcopy();
    end();
  }
  void toggleRepeatDestination() {
    repeatPCtoNextion = !repeatPCtoNextion;
    introduction();
    aboutPCcopy();
    end();
  }
  void setPCBaud(long baud) {
    introduction();
    Serial.print("changing PC baud rate to ");
    Serial.print(baud);
    end();
    Serial.end();
    ports[0].baudRate = baud;
    Serial.begin(baud);
  }
  void toggleDebug() {
    debug = !debug;
    introduction();
    Serial.print("debug=");
    Serial.println(debug);
  }
public:
  /*Commands:
@p ping (not catually)
@s status
@n toggle Nextion mute
@b<h,l> set high/low baud rate with PC (115200, 9600) 
@r<n,p,d> toggle repeat options n-Nextion to PC, p-PC repeat, d-PC repeat destination(Nextion or State);
*/
  uint8_t validateBuffer(ValidBuffer* buffer) {
    buffer->reset();
    if (buffer->available())
      switch (buffer->read()) {  //char 0 only @!
        case '@':
          if (buffer->available())
            switch (buffer->read()) {  //char 1 @
              case 'b':
                if (buffer->available())
                  switch (buffer->read()) {  //char 2 @b
                    case 'h':
                      setPCBaud(115200);
                      return 2;
                    case 'l':
                      setPCBaud(9600);
                      return 2;
                    default:;  //invalid
                  }
                else return 1;  //not complete
              case 'p':         //@p -ping
                Serial.println("PostMan: PING!");
                return (2);  //complete command
              case 'n':      //toggle nextion mute
                toggleNextionMute();
                return (2);
              case 'r':  //repeat
                if (buffer->available())
                  switch (buffer->read()) {  //char 2 @r
                    case 'n':                //toggle nextion repeat
                      toggleNextionRepeat();
                      return (2);
                    case 'p':  //toggle pc repeat
                      togglePCRepeat();
                      return (2);
                    case 'd':  //toggle pc repeat destination
                      toggleRepeatDestination();
                      return 2;
                    default:;  //invalid
                  }
                else return (1);  //not enoigh symbols
              case 's':
                status();
                return (2);
              case 'd':
                toggleDebug();
                return (2);
              default:  //not valid
                ;
            }
          else return (1);  //not enoigh symbols
        default:            //not valid
          ;
      }
    return 0;
  }
  uint8_t peekPort(Port* port, uint8_t i) {  //peek port and try to validate by recievers;
    ValidBuffer* buffer = &port->buffer;     //assign pointer to buffer
    HardwareSerial* sport = port->port;      //assign pointer to serial port
    char symbol = sport->peek();
    buffer->add(symbol);               //add peeked symbol to buffer
    if (port->lastValidReciever != 2)  //not incomplete rS command, empty or incomplete PM command
      switch (validateBuffer(buffer)) {
        case 1:  //incomplete PM command
          port->lastValidReciever = 1;
          return 1;
        case 2:  //complete PM command
          return 2;
        default:  //0, let rS try validate symbol
          buffer->erase();
          buffer->add(symbol);
      }
    uint8_t result;
    if ((i && muteNextion) || (!i && (!repeatPC || repeatPCtoNextion)))
      //if Serial1 and muted or just Serial and (not repeating or toNextion instead of state)
      //abort rS validation, assume invalid
      result = 0;
    else
      result = robotState->validateBuffer(buffer);  //result of rS validation
    switch (result) {
      case 0:                           //invalid rS command
        if (symbol == '@') {            //assuming lVR=2 and there was no check, if symbol is the beginning of PM command
          port->lastValidReciever = 1;  //incomplete PM command
          buffer->erase();
          buffer->add(symbol);
          return (1);
        }
      case 1:
        port->lastValidReciever = 2;  //incomplete rS command
      default:;
    }
    return (result);
  }
  void update() {
    for (uint8_t i = 0; i < 2; i++) {       //foreach serial port 0, 1
      Port* port = &ports[i];               //Port pointer
      HardwareSerial* sport = port->port;   //serial port pointer
      ValidBuffer* buffer = &port->buffer;  //buffer pointer

      if (!(*sport)) {
        if (port->tryConnect)
          port->begin();
        if (port->connectionRequired) {
          while (!(*sport)) {
            port->begin();
          }
        }
      }
      bool unusedSymbol = false;
      if (sport->available()) {  //if there any char in  serial buffer
        // if (debug) {
        //   buffer->print();
        // }
        switch (peekPort(port, i)) {  //peek and validate with set restrictions
          case 1:
            buffer->reset();
            break;
          case 0:
            unusedSymbol = true;
          default:  //2
            port->lastValidReciever = 0;
            buffer->erase();
        }
        switch (i) {  //retranslation act
          case 0:
            if (repeatPCtoNextion && repeatPC && unusedSymbol)  //if repeat to nextion and last symbol wasn't used
              if (Serial.peek() == '`') {                       // Command termination symbol
                if (debug) Serial.println("Debug: Nextion termination send");
                comEnd();  //write
              } else {
                if (debug) Serial.write(Serial.peek());
                Serial1.write(Serial.peek());
              }
            break;
          case 1:
            if (repeatNextion) {
              Serial.write(Serial1.peek());
            }
          default:;
        }
        sport->read();
      }  //available
    }    //for
  }
};