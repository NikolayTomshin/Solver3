#include "Path.h"

namespace SCS {

uint8_t getPostOpearationIndex(uint8_t originIndex, Operation op) {
  switch (Mod(4, op.ortoAngle)) {
    default: return (originIndex);
    case 1:
      // Serial.println("CW 1oA");
      return (Space[originIndex].getLink(op.ortoVector));
    case 2:
      // Serial.println("2oA");
      return (Space[Space[originIndex].getLink(op.ortoVector)].getLink(op.ortoVector));
    case 3:
      // Serial.println("CCW 1oA");
      return (Space[originIndex].getLink((op.ortoVector + 3) % 6));
  }
}

}  //ns SCS

OperationPathStack getShortestPathEdge(uint8_t beginningScindex) {
  OperationPathStack opPath = OperationPathStack(new EdgeOperationNode(beginningScindex, 0));  //add required beginning zero operation

  if (beginningScindex) {  //if not already solved

    for (uint8_t depth = 1; depth < 4; depth++) {  //search start
      opPath.nodeFromLast()->reset();
      /*reset root node after depth increase
      (which happens after full exploration of root to depth level or at the start and does nothing)*/
loop:
      while (opPath.length < depth) {  //while not enought depth add unexplored branches
        opPath.addUnexplored();
        // Serial.print("add 1");  //
        // sPnl();                 //
      }
      // Serial.print("Currently at");                          //
      // Serial.print(opPath.nodeFromLast()->scindex);         //
      // SCS::getCsT(opPath.nodeFromLast()->scindex).print();  //
      // sPnl();                                                //

      do {  //Search branches of this node
        // opPath.nodeFromLast()->print();  //
        if (opPath.nodeFromLast()->getPostOPI() == 0) {
          // Serial.print("YES!");  //
          goto stop;
        }  //if found 0
      } while (opPath.nextIfPossibleForLast());

      // Serial.print("fail");         //
      // sPnl();                       //
      while (opPath.length > 1) {  //this was not the first node explored root is kept from deletion
        if (opPath.nextIfPossibleForLast()) {
          goto loop;
        } else {
          opPath.removeNode();
        }
      }  //inc or remove explored node
    }    //depth
  }      //if != 0
  else {
    opPath.removeNode();
  }
stop:
  return (opPath);
}