// #include "Vec.h"
// #include "Mathclasses.h"
// #include "TimeManager.h"
// #include <stdio.h>
// #include "CsT.h"
// #include "SCs.h"

// void setup() {
//   Serial.begin(9600);
//   Serial.println();

//   CsT* Arr[24];
//   uint8_t CI = 0;  //current index
//   int MI = 0;
//   int8_t Links[24][6]{ { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 },
//                        { -1, -1, -1, -1, -1, -1 } };  //array for links

//   Arr[0] = new CsT(0, 1, 2);  //первая система
//   Arr[0]->Print();
//   while (MI < 23) {

//     for (CI = 0; CI <= MI; CI++) {


//       for (int OV = 0; OV < 6; OV++) {

//         CsT CurCs(*Arr[CI]);
//         CurCs.Rotate(OV, 1);
//         bool NoMatch = true;
//         for (int tI = 0; tI <= MI; tI++) {

//           if (CurCs.Compare(Arr[tI])) {
//             NoMatch = false;
//             break;
//           }
//         }
//         if (NoMatch) {
//           MI++;
//           Serial.println();
//           CurCs.Print();
//           Arr[MI] = new CsT(CurCs);
//         }
//       }
//     }
//     Serial.print("New upper limit");
//     Serial.println(MI);
//   }
//   for (int i = 0; i < 23; i++)
//     for (int j = i + 1; j < 24; j++)
//       if (Arr[i]->Compare(Arr[j])) {
//         Serial.println("REPEATING!");
//         goto herewego;
//       }
//   Serial.println("All unique!");
// herewego:
//   for (int i = 0; i < 24; i++) {
//     Serial.print("Links for CS");
//     Serial.print(i);
//     Arr[i]->Print();
//     Serial.println(":");
//     for (int j = 0; j < 6; j++) {
//       if (Links[i][j] < 0) {
//         CsT CurCs(*Arr[i]);
//         CurCs.Rotate(j, 1);
//         for (int ti = 0; ti < 24; ti++) {
//           if (CurCs.Compare(Arr[ti])) {
//             Links[i][j] = ti;

//             Serial.print(j);
//             Arr[ti]->Print();
//             Serial.print("; ");
//             break;
//           }
//         }
//       }
//     }
//     Serial.println();
//   }
//   for (int i = 0; i < 24; i++) {
// keepgoing:
//     for (int j = 0; j < 5; j++)
//       for (int k = j + 1; k < 6; k++)
//         if (Links[j] == Links[k]) {
//           Serial.print("LINE ");
//           Serial.print(i);
//           Serial.print(" NOT UNIQUE ");
//           Serial.print(k);
//           Serial.print(" == ");
//           Serial.println(j);
//           i++;
//           goto keepgoing;
//         }
//     Serial.print("Line ");
//     Serial.print(i);
//     Serial.println(" all unique ");
//   }
// //   SCs* aray[24];
// //   for (int i = 0; i < 24; i++) {
// //     aray[i] = new SCs(Arr[i]->ON[0], Arr[i]->ON[1], Arr[i]->ON[2], Links[i][0], Links[i][1], Links[i][2], Links[i][3], Links[i][4], Links[i][5]);
// //  Serial.print("SCs");    
// //     aray[i]->PrintPars();
// //     Serial.println(",");
// //   }
// }