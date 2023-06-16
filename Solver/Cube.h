#pragma once
#include "Path.h"
//file for dealing with virtual cube

namespace Cube {

uint8_t linearIndex(Vec v);

const uint8_t edgeLindex[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, 14, 15 };  //linear indexes of edge pieces

Vec unfoldLinIndex(uint8_t linindex);

extern Piece RealPieces[20];  //real cube pieces 20 подвижных деталек которые двигаются в системе куба

void resetRealPieces();  //each piece on it's default pos and def pos set as unfolded linear index

struct State {
  uint8_t scs[13];                        //compressed array of SCS indexes of pieces ordered by real array
  uint8_t coordinateArrangedIndexes[13];  //compressed array of real pieces indexes in order of 3d lineared coordinates
  inline static uint8_t cAIcopy[13];      //temp cAI copy for applying operations

  State();
  uint8_t getscs(uint8_t index);
  void setscs(uint8_t pieceIndex, uint8_t newSCsIndex);
  void showscs();                                        //print scs in string
  void setCAI(uint8_t pieceindex, uint8_t linearindex);  //задать ссылку индексом детали по линейным координатам
  void updateCAI();                                      //обновление массива индексов по координатам
  uint8_t getCAI(uint8_t index);                         //get CAI value(piece index) by linear index
  void showcai();                                        //print cai string
  uint8_t getscsByVec(Vec vec);                          //get scs index of piece by vector in this state
  static uint8_t getCAIcopy(uint8_t index);              //get CAI value(piece index) by linear index from copied array
  void applyOperation(Operation op);                     //modify state's scs and CAI according to operation(rotation of one side)
  void printSliced(bool piOrSc, CsT cubeCs = CsT());     //show scs kinda in 3d
  static uint8_t edgePenaltyByDefault(uint8_t pieceindex);
  uint8_t statePenalty();  //if 1; ep=0; (1+epbd)*lp[1..3]


};  //State
struct Color {
  uint16_t component[3];
};

extern uint8_t* indexedTiles;
extern Color* colorTiles;     //colors for all tiles
extern Color colorPallet[6];  //6 x color

void initializeColors();
void setColorTile(uint16_t r, uint16_t g, uint16_t b);
//scanner sets colorTiles
void indexColors();
void setPieces();
void deleteColors();

}  //ns Cube
