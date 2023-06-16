#pragma once
#include "Stack.h"
#include "Arduino.h"
#include <stdint.h>

template<class StepType>
struct ISearchNode {  //Search node interface
  // virtual ~ISearchNode();
  virtual kill();
  virtual StepType stepValue();                 //get Value of step
  virtual void print();                         //print for debug
  virtual ISearchNode<StepType>* unexplored();  //create new next unexplored node
  virtual void reset();                         //reset counter anything without deleting
};

template<class NodeType>
struct PathStack : public StackHandler<NodeType*> {  //Stack of node Pointers
  void print() {                                     //call print() for each NodeType* in element value
    if (this->length > 0) {
      this->temp = this->last;
      while (this->temp != NULL) {  //while temp exist
        this->temp->value->print();
        this->temp = this->temp->previous;
      }
    } else {
      Serial.print("[empty]");
    }
  }
  NodeType* nodeFromLast(uint8_t i = 0) {
    return (*(StackHandler<NodeType*>::valueFromLast(i)));  //evaluate pointer to node pointer
  }
  void removeNode() {
    this->last->value->kill();
    // Serial.print(":/");//
    StackHandler<NodeType*>::remove();
  }
  clearNodes() {
    while ((this->length) > 0) {
      removeNode();
    }
  }
  ~PathStack() {
    clearNodes();
  }
};

struct IOperationNode : public ISearchNode<Operation> {  //Search Node for operation steps
  uint8_t counter;                                       //has counter
  uint8_t scindex;                                       //has strating index
  virtual IOperationNode() {}
  virtual IOperationNode(uint8_t ind) {
    scindex = ind;
  }
  virtual IOperationNode(uint8_t code, uint8_t ind)
    : IOperationNode(ind) {
    counter = code;
  }
  virtual bool nextIfPossible();
  uint8_t getPostOPI() {
    return SCS::getPostOpearationIndex(scindex, stepValue());
  }
  void print() override {
    stepValue().print();
  }
  void reset() override {
    counter = 0;
  }
};

struct OperationNode : public IOperationNode {  //Any operation Node
  OperationNode() {}
  OperationNode::~OperationNode() {
    // Serial.print("Bye!");
  }
  OperationNode::OperationNode(uint8_t ind) {
    scindex = ind;
  }
  OperationNode::OperationNode(uint8_t ind, uint8_t code) {
    counter = code;
    scindex = ind;
  }
  virtual kill() {
    // Serial.print("killing");
    OperationNode* a = this;
    delete a;
  }
  bool nextIfPossible() override {
    // Serial.print("NO THAT'S WRONG!"); //
    if (counter < 8) {
      counter++;
      return true;
    } else {
      return false;
    }
  }
  Operation stepValue() override {
    return (Operation(counter / 3, counter % 3 + 1));
  }
  OperationNode* unexplored() override {
    return (new OperationNode(getPostOPI(), 0));
  }
};

struct EdgeOperationNode : public IOperationNode {  //SearchNode for edge operations//counter is 0..5 axis(0..2 -1)*angle(1..3)->0..1*0..2
  EdgeOperationNode::EdgeOperationNode() {}
  EdgeOperationNode::~EdgeOperationNode() {
    // Serial.print("ByeBye!!");
  }
  EdgeOperationNode::EdgeOperationNode(uint8_t ind) {
    scindex = ind;
  }
  EdgeOperationNode::EdgeOperationNode(uint8_t ind, uint8_t code) {
    counter = code;
    // Serial.print(counter);//
    // sPnl();  //
    scindex = ind;
    // Serial.println(scindex);//
  }
    virtual kill() {

    // Serial.print("killing");
    EdgeOperationNode* a = this;
    delete a;
  }
  Operation stepValue() override {                           //get current step(operation)
    uint8_t ov = counter / 3;                                //get unedited axis undex
    ov += (SCS::getCsT(scindex).getComponent(1) % 3 <= ov);  //exclude impossible axis
    // Serial.print(counter);                                   //
    // Serial.print(",");                                       //
    // Serial.println((counter % 3) + 1);                       //
    return (Operation(ov, (counter % 3) + 1));
  }
  bool nextIfPossible() override {
    if (counter < 5) {
      counter++;
      return (true);
    }
    return (false);
  }
  EdgeOperationNode* unexplored() {
    return (new EdgeOperationNode(getPostOPI(), 0));
  }
};


struct OperationPathStack : public PathStack<IOperationNode> {
  OperationPathStack(IOperationNode* root) {
    add(root);
  }
  void addUnexplored() {
    if (length > 0) {
      add((*valueFromLast(0))->unexplored());  //add()
    } else {
      Serial.print("EMPTY CAN'T ADD!!!");
    }
  }
  bool nextIfPossibleForLast() {
    return (nodeFromLast()->nextIfPossible());
  }
};



