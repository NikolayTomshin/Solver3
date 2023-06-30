#include <string.h>
#include <avr/pgmspace.h>
#pragma once
#include "USBAPI.h"
#include <stdint.h>
#include "ClawUnit.h"

ClawUnit n1;  //left claw 1
ClawUnit n2;  //right claw 2
Timer nextionTimer(10);

CsT scannerCs = CsT(5, 0, 1);  //x-смотрит на куб, y-в сторону откидывания
class Scanner {
public:
  Servo servo;
  bool invertAngles = false;
  uint8_t servoAngles[4] = { 5, 143, 153, 167 };  //folded 0,1,2,3 centre
  // uint8_t positionCode;//unused
  // void setPosition(uint8_t posCode){//unused
  // }
  Scanner() {}
  Scanner(uint8_t sC) {
    servo.attach(sC);
  }
  void goAngle(uint8_t degrees) {
    uint8_t angle;
    if (invertAngles) {
      angle = 180 - degrees;
    } else {
      angle = degrees;
    }
    servo.write(angle);
  }
  void goPosition(uint8_t position) {
    goAngle(servoAngles[position]);
  }
  void saveSettings();
  void loadSettings();
};
Scanner scanner;

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

char comPars[2];      //command parameters
class ICommandable {  //has command masks and can
public:
  virtual uint8_t numberOfCommands();
  virtual char* getString(uint8_t index);
  virtual void doCommand(uint8_t index);
};

bool isNumber(uint8_t kar, uint8_t lowerBound = '0', uint8_t upperBound = '9') {  //check if char is a number between
  return ((lowerBound <= kar) && (kar <= upperBound));
}
int8_t indexOf(char* charPointer, char character, uint8_t startingIndex = 0) {
  int8_t index = -1;
  uint8_t length = strlen(charPointer);
  for (uint8_t i = startingIndex; i < length; i++) {
    if (charPointer[i] == 'character') {
      index = i;
      break;
    }
  }
  return index;
}
uint8_t comMaskLength(char* mask) {  //actual length of the required command
  uint8_t maskLength = strlen(mask);
  uint8_t comLength = maskLength;
  for (uint8_t index = 0; index < maskLength; index++) {
    switch (mask[index]) {
      case '[':  //[<lower bound><upper bound>
        comLength -= 2;
        index += 2;
        break;
      case '`':  //substract rest symbols= number of options + 1'`'
        uint8_t step = indexOf(mask[index], '`', 1) + 1;
        comLength -= step;
        index += step + 1;
      default:;
    }
  }
  return (comLength);
}


bool globalShowCommands = false;
const uint8_t bufferSize = 5;
struct ValidBuffer {   //buffer for validated chars
  uint8_t length = 0;  //length of
  uint8_t cursor = 0;  //index of starting letter
  char letters[bufferSize];
  void print(uint8_t shift = 0) {
    Serial.print('[');
    for (uint8_t i = shift; i < length; i++) {
      Serial.write(getLetter(i));
    }
    Serial.print(']');
  }
  uint8_t modIndex(int8_t shift) {  //index of letter relative to cursor
    return Mod(bufferSize, cursor + shift);
  }
  uint8_t freeSpace() {
    return (bufferSize - length);
  }
  void add(char value) {  //move cursor and overwrite letter
    if (length < bufferSize) {
      letters[modIndex(length)] = value;
      length++;  //increase length = shift relative to cursor
    } else {
      letters[cursor] = value;  //new letter overwrites first
      cursor = modIndex(1);     //move cursor right,
    }
  }
  char getLetter(uint8_t localIndex) {
    // Serial.print("(" + String(modIndex(localIndex)) + ")");
    return letters[modIndex(localIndex)];
  }
  void erase(uint8_t lettersToErase = 255) {
    // if (globalShowCommands) {
    //   Serial.print("ERSD:");
    //   for (uint8_t i = 0; (i < length) && (i < lettersToErase); i++)
    //     Serial.write(getLetter(i));
    //   Serial.println();
    // }
    if (lettersToErase < length) {
      length -= lettersToErase;
      cursor = modIndex(lettersToErase);
    } else length = 0;
  }
  int8_t localIndexOf(char character, uint8_t startingIndex = 0) {
    int8_t index = -1;
    for (uint8_t i = startingIndex; i < length; i++) {
      if (getLetter(i) == 'character') {
        index = i;
        break;
      }
    }
    return index;
  }
  uint8_t cast(char* mask, uint8_t shift = 0) {  //check if buffer contains command like mask (shifted by cursor)
    uint8_t parIndex = 0;
    uint8_t maskLength = strlen(mask);  //length of  mask
    // Serial.print("ML=");
    // Serial.print(maskLength);
    uint8_t i = 0, j = 0;
    uint8_t limit = length - shift;
noOptionsHappened:
    while ((i < limit) && (j < maskLength)) {
      char thisLetter = getLetter(i + shift);
      char maskChar = mask[j];
      Serial.print(maskChar);
      switch (maskChar) {
        case '[':  // '[' 'lower bound' 'upper bound'
          Serial.print('[');
          if ((limit - i) < 4)  //enough symbols left for check
          {
            if (!isNumber(thisLetter, mask[j + 1], mask[j + 2]))  //do check
              return 0;                                           //not this command
            comPars[parIndex] = thisLetter;
            parIndex++;
          }
          j += 3;  //step over 2 mask symbols anyway
          break;
        case '#':  //save as parameter
          Serial.print('#');
          comPars[parIndex] = thisLetter;
          parIndex++;
          j++;  //next mask symbol
          break;
        case '`':  //mask[j]<>...<>'`'
          Serial.print('`');
          j++;                                             //step on options
          int8_t numberOfOptions = indexOf(mask[j], '`');  //number of options, possibly 0 but why ever?
          if (numberOfOptions < 0) return 0;               //broken mask
          bool match = false;
          if (numberOfOptions > 0) {                         //if options exist
            for (uint8_t o = 0; o < numberOfOptions; o++) {  //search for match
              match = (thisLetter == mask[j + o]);           //through all [o]ptions
              if (match) break;
            }
          } else goto noOptionsHappened;
          if (!match) return 0;  //not matching
          comPars[parIndex] = thisLetter;
          parIndex++;
          j += numberOfOptions + 1;  //skip every option + closing '`'
          break;
        default:  //any other letter directly compares
          Serial.print('A');
          Serial.print(maskChar);
          if (thisLetter != maskChar) return 0;
          j++;  //next mask symbol
          
      }
      i++;  //next buffer letter
    }
    if (j >= maskLength) {       //if aaplied
      if (globalShowCommands) {  //show command
        Serial.print(mask);
        Serial.print("\t[");  //open bracket
        i = shift;
        for (j = 0; j < maskLength; i++) {  //print one symbol each time
          char maskChar = mask[j];
          if (maskChar == '#') {  //single numerical parameter
            Serial.write('<');
            Serial.print(uint8_t(getLetter(i)));
            Serial.write('>');
            j++;
          } else {  //char as is
            Serial.write(getLetter(i));
            if (maskChar == '[') {  //
              j += 3;
            } else if (maskChar == '`') {
              j += indexOf(mask[j], '`', 1) + 1;
            } else
              j++;
          }
        }
        Serial.print("]");
        if (parIndex > 0) {
          Serial.print("\t(");
          for (uint8_t i = 0; i < parIndex; i++) {
            Serial.print(uint8_t(comPars[i]));
            if (i < (parIndex - 1))
              Serial.print(",\t");
          }
          Serial.write(')');
        }
      }
      return (2);
    }
    return (1);
  }
  uint8_t checkCommands(ICommandable* commandable, bool eraseIfNotApplied) {  //check and execute commands of commandable, optionally erase unused chars

    //returns 1 if buffer contains beginning of command, 0 if not, and 1+number of executed commands if buffer contained correct commands
    uint8_t executionCount = 0;
    uint8_t numberOfCommands = commandable->numberOfCommands();  //limit of c-command indexes
    uint8_t iterationHighest = 0;
    bool anyApplied = false;              //during check any applied
    for (uint8_t i = 0; length - i > 0;)  //iteration(shift) while symbols available attempt matching
    {
      iterationHighest = 0;
      for (uint8_t c = 0; c < numberOfCommands; c++) {  //foreach command
        char* commandString = commandable->getString(c);
        Serial.print(commandString);
        uint8_t castResult = cast(commandString, i);
        if (castResult > iterationHighest) iterationHighest = castResult;
        switch (castResult) {
          case 2:
            Serial.print("\t<=\t");
            print(i);
            Serial.write('\n');
            if (globalShowCommands) {
              Serial.write("\t");
              Serial.println(c);
            }
            commandable->doCommand(c);  //actually do comand
            executionCount++;
            erase(comMaskLength(commandString) + i);  //erasing all used and previous letters
            goto afterCompleteCommand;
          case 1:  //incoplete
            Serial.print("\t>\t");
            print(i);
            Serial.write('\n');
            break;
          default:;  //wrong
            Serial.print("\t!=\t");
            print(i);
            Serial.write('\n');
        }
      }  //iteration check ended

      if (iterationHighest == 1) return 1;         //if incoplete command end with this
      if (eraseIfNotApplied && !iterationHighest)  //if unused erasure allowed and previous iteration didn't applied
        erase(1);                                  //erase unused symbol and begin new iteration with same i
      else i++;                                    //else iterate as usual
afterCompleteCommand:
      if (!anyApplied) anyApplied = iterationHighest;  //update application
    }
    return (anyApplied + executionCount);
  }
};

struct Port {
  unsigned long baudRate = 9600;
  bool tryConnect = true;
  bool connectionRequired = false;
  ValidBuffer buffer;
  HardwareSerial* port;
  Port(HardwareSerial* theport) {
    port = theport;
  }
  void begin(unsigned long _baudRate) {
    baudRate = _baudRate;
    port->begin(_baudRate);
  }
};

class IRstate : public ICommandable, public IUpdatable {  //class that can take commands, can operate all robot hardware on Update basis
public:
  virtual void reactivate() {}
};
IRstate* robotState;
struct StateStack : public StackHandler<IRstate*> {
  StateStack()
    : StackHandler() {}
  void updCurrentState();
  void addState(IRstate* value);
  void removeState();
};  //stack of robot states
StateStack stateStack;
void StateStack::updCurrentState() {
  robotState = last->value;
}
void StateStack::addState(IRstate* value) {
  StackHandler<IRstate*>::add(value);
  updCurrentState();
}
void StateStack::removeState() {
  StackHandler<IRstate*>::remove();
  updCurrentState();
  robotState->reactivate();
}

/*About Serial connection. robotState can interprete custom commands specified for it recieved from interface device.
  PostMan class manages Serial communication between interface device ID, robot state RS and external device (PC)  
  PC<->PM(->RS->ID),(<->ID)
  RS sends commands on it's own and isn't being filtered                              RS->ID
  ID (Serial1) sends commands through PostMan, may be filtered or repeated to PC      ID->PM->RS,PC
  PC (Serial) sends commands to PostMan, may be repeated to RS or ID                  PC->PM->RS,ID
  
  RS has number of acceptable commands of specific length
  PM has number of specific commands that won't be retranslated if detected, used for debug from PC
  ID(Nextion) commands are send directly from RS or retranslated from PC via PM
  
  PostMan has compacted boolean variables [0:mute ID, 1:enable repeat, 2:repeat to 0=RS/1=ID] //!!!OLD COMMENT
  Again, postman recives it's commands only from PC and won't repeat them even when repat is available  */
void comEnd() {  //Nextion command rermination
  Serial1.print("\xFF\xFF\xFF");
}

class PostMan : public ICommandable, public IUpdatable {
  bool repeatPC = false;          //repeat input from serial to Nextion or RS
  bool repeatPCtoNextion = true;  //true Nextion, false - RS
  bool repeatNextion = false;
  bool muteNextion = false;
  bool showCommands = true;
  const char commands[7][4] = {
    "@p",   //0 ping (not catually)
    "@s",   //1 status
    "@n",   //2 toggle Nextion mute
    "@d",   //3 toggle debug
    "@b#",  //4 set high/low baud rate with PC (115200, 9600) @b<h,l>
    "@r#",  //5 toggle repeat options n-Nextion to PC, p-PC repeat, d-PC repeat destination(Nextion or State); <n,p,d>
    "@c"    //6 repeat commands before termination
  };
  uint8_t numberOfCommands() override {
    return 7;
  }
  char* getString(uint8_t index) override {
    return &commands[index][0];
  }
  void doCommand(uint8_t index) override {
    unsigned long ba;
    switch (index) {
      case 0:
        Serial.println("Ping ping!");
        break;
      case 1:
        status();
        break;
      case 2:
        toggleNextionMute();
        break;
      case 3:
        //toggleDebug();
        break;
      case 4:
        if (comPars[0] == 'h') ba = 115200;
        else ba = 9600;
        setPCBaud(ba);
        break;
      case 5:
        switch (comPars[0]) {
          case 'p':
            togglePCRepeat();
            break;
          case 'd':
            toggleRepeatDestination();
            break;
          case 'n':
            toggleNextionRepeat();
            break;
        }
        break;
      case 6:
        toggleShowCommands();
        break;
      default:;
    }
  }
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
  void toggleShowCommands() {
    showCommands = !showCommands;
    globalShowCommands = showCommands;
    Serial.print("Show commands o");
    if (showCommands) Serial.println("n");
    else Serial.println("ff");
  }
public:
  PostMan() {
    globalShowCommands = showCommands;
  }
  void reactToPort(ValidBuffer* buffer, HardwareSerial* sport) {
    if (sport->available()) {
      buffer->add(sport->peek());
      // buffer->print();
      if (!buffer->checkCommands(this, false))  //check postman commands
        buffer->checkCommands(robotState, true);
    }
  }
  void update() {
    for (uint8_t p = 0; p < 2; p++) {
      Port* port = &ports[p];
      HardwareSerial* sport = port->port;
      if (sport->available()) {
        ValidBuffer* buffer = &port->buffer;
        // Serial.print('>');
        // Serial.write(sport->peek());
        if (!p || !muteNextion)  //prevent reaction to S1 if nextion muted
          reactToPort(buffer, sport);
        if (p) {  // Serial1
          if (repeatNextion) {
            Serial.write(Serial1.peek());
          }
        } else {  //Serial(0)
          if (repeatPC) {
            if (repeatPCtoNextion) {
              char temp = Serial.peek();
              if (temp == '`')
                comEnd();  //send command end
              else
                Serial1.write(temp);  //repeat to nextion
            } else {                  //repeat to robotState
              ports[1].buffer.add(Serial.peek());
            }
          }
        }
        sport->read();
      }  //available
    }
  }
};
//nextion work
void loadSomething(String objName, String propName, String value, bool brackets = false) {  //{objName}.{propName}={value}
  comEnd();
  Serial1.print(objName);
  Serial1.write('.');
  Serial1.print(propName);
  Serial1.write('=');
  if (brackets) {
    Serial1.write('"');
    Serial1.print(value);
    Serial1.write('"');
  } else
    Serial1.print(value);
  comEnd();
}
void loadTxt(String name, String txt) {  //update text  {name}.txt="{txt}"
  loadSomething(name, "txt", txt, true);
}
void loadVal(String name, uint16_t val) {  //{name}.val={val}
  loadSomething(name, "val", String(val), false);
}
void callFunction(String name, String par0 = "") {  //{name} {pars}
  comEnd();
  Serial1.print(name);
  Serial1.write(' ');
  Serial1.print(par0);
}
void addParametre(String par) {
  Serial1.write(',');
  Serial1.print(par);
}
void click(String name, bool press) {  //click {name},[press/release]
  callFunction("click", name);
  addParametre(String(press));
  comEnd();
}
void goPage(String pagename) {
  callFunction("page", pagename);
  comEnd();
}
String letterIndex(String letter, uint8_t index) {
  return (letter + String(index));
}
void loadSlider(uint8_t number, uint8_t value) {
  String name = letterIndex("h", number);
  loadVal(name, value);
  click(name, false);
}

PostMan router;
void hardUpdate() {
  n2.update();
  n1.update();
}
void serialUpd() {
  if (nextionTimer.isLoop()) router.update();
}
void stateUpd() {
  robotState->update();
}
enum popupType {
  err,
  warn
};

class Waiting : public IRstate {
  char mask[bufferSize + 1];
public:
  bool timaHasCome = false;
  uint8_t numberOfCommands() override {
    return 1;
  }
  char* getString(uint8_t index) override {
    return &mask[0];
  }
  virtual void doCommand(uint8_t index) override {
    timaHasCome = true;
  }
  Waiting(String _mask) {
    _mask.toCharArray(&mask[0], bufferSize + 1);
  }
};
void wait(String _mask) {
  Waiting* a = new Waiting(_mask);
  stateStack.addState(a);
  while (!a->timaHasCome) serialUpd();
  stateStack.removeState();
}
void popup(popupType pop, String title = "", String desc = "") {
  switch (pop) {
    case err:
      goPage("err");
      break;
    case warn:
      goPage("warn");
      break;
    default: return void();
  }
  if (title.length() > 0) loadTxt("t0", title);
  if (desc.length() > 0) loadTxt("t1", desc);
  wait("O");
}