#include "Str.h"

uint16_t StrRep::LenSizeProxy::getSize() {
  return ref.strLen();
}
uint16_t StrRep::strLen() const {
  uint16_t len = 0;
  {
    auto itp = iteratorForwardV();
    for (auto& it = *itp; it.notEnd(); ++it) {
      if (*it == '\n') break;
      ++len;
    }
  }
  return len;
}
SPtr<StrRep::RIterator> StrRep::iteratorForwardV() {
  return SPtr<RIterator>(new DumbRiterator(*this));
}
void StrRep::toCharArray(char* pointer, uint16_t size) {
  char* limit = pointer + min(strLen(), size);  //array limit
  auto itp iteratorForwardV();                  //init iterator
  auto& it = *itp;
  while (pointer < limit) {  //foreach char in array
    *pointer = *it;          //copy char
    ++pointer;               //++pointer
    ++it;                    //iterate to next
  }
  *pointer = '\0';  //put end
}
String StrRep::toString() {
  uint16_t len = strLen();
  String val;                   //init string
  val.reserve(len);             //set size
  auto itp iteratorForwardV();  //init iterator
  auto& it = *itp;
  for (uint16_t i = 0; i < len; ++i) {  //foreach char in strrep
    val[i] = *it;                       //copy char
    ++it;                               //iterate to next //++i
  }
  return val;  //return val str
}
StrVal StrRep::toVal() {
  uint16_t len = strLen();
  StrVal str(len);
  {
    auto itp = iteratorForwardV();
    auto& it = *itp;
    for (uint16_t i = 0; i < len; ++i) {
      str[i] = *it;
      ++it;
    }
  }
  return str;
}
StrVal StrRep::toValFull() {
  uint16_t size = getSize();
  StrVal str(size);
  {
    auto itp = iteratorForwardV();
    auto& it = *itp;
    for (uint16_t i = 0; i < size; ++i) {
      str[i] = *it;
      ++it;
    }
  }
  return str;
}
void StrRep::print(HardwareSerial& port) {
  auto itp = iteratorForwardV();
  for (auto& it = *itp; it.notEnd(); ++it) {
    char c = *it;
    if (c == '\0') return;
    port.write(c);
  }
}
void StrRep::printn(HardwareSerial& port) {
  print(port);
  port.write('\n');
}
void StrRep::printFull(HardwareSerial& port) {
  auto itp = iteratorForwardV();
  for (auto& it = *itp; it.notEnd(); ++it)
    port.write(*it);
}
void StrRep::printnFull(HardwareSerial& port) {
  printFull(port);
  port.write('\n');
}
int16_t StrRep::indexOf(const StrRep& sample, uint16_t from = 0) {
  uint16_t limit;
  {
    uint16_t sLen = sample.strLen();
    if (!sLen) return -1;
    limit = from + sLen;  //required minimal length for single comparison
  }
  uint16_t len = strLen();              //len of this string
  if (limit > len) return -1;           //can't compare and find any
  limit = size + 1 - limit;             //now limit is how much comparisons can be made
  auto masterItp = iteratorForwardV();  //main master Iterator pointer
  auto& masterIt = *masterItp;          //main master iterator reference
  mIt.skip(from);                       //prepare mIt for first comparison from from
  for (uint16_t comparedTimes = 0; comparedTimes < limit; ++comparedTimes) {
    auto mItp = masterIt.clone();           //main Iterator copy pointer
    auto& mIt = *mITp;                      //main Iterator copy reference
    auto sItp = sample.iteratorForwardV();  //sample Iterator pointer
    auto& sIt = *sItp;                      //sample Iterator reference
    while (sIt.notEnd()) {                  //comparison loop
      if (*mIt != *sIt)
        goto nestedFailed;  //if not equal exit and continue outer loop
      ++mIt;
      ++sIt;  //if equal compare next pair of chars
    }
    return from + comparedTimes;  //if all equal return index
nestedFailed:
    ++masterIt;  //++master Iterator //++comparedTimes and repeat
  }
  return -1;  //comparedTinmes reached limit and equal sequence wasn't found
}
int16_t StrRep::indexOfLast(const StrRep& sample, uint16_t backFrom = 0xFFFF) {
  uint16_t limit = sample.strLen();       //sample's length
  if (!limit) return -1;                  //can't search for empty
  uLimitedStrict(backFrom, strLen());     //backFrom limited strictly by this len
  if (backFrom < (limit - 1)) return -1;  //can't compare and find any
  limit = backFrom + 2 - limit;           //number of posiible comparisons from beginning
  auto masterItp = iteratorForwardV();    //main master Iterator pointer
  auto& masterIt = *masterItp;            //main master iterator reference
  int16_t lastMatchIndex = -1;            //not found match yet
  for (uint16_t comparedTimes = 0; comparedTimes < limit; ++comparedTimes) {
    auto mItp = masterIt.clone();           //main Iterator copy pointer
    auto& mIt = *mITp;                      //main Iterator copy reference
    auto sItp = sample.iteratorForwardV();  //sample Iterator pointer
    auto& sIt = *sItp;                      //sample Iterator reference
    while (sIt.notEnd()) {                  //comparison loop
      if (*mIt != *sIt)
        goto nestedFailed;  //if not equal exit and continue outer loop
      ++mIt;
      ++sIt;  //if equal compare next pair of chars
    }
    lastMatchIndex = comparedTimes;  //if all equal, update last match index
nestedFailed:
    ++masterIt;  //++master Iterator //++comparedTimes and repeat
  }
  return lastMatchIndex;  //comparedTinmes reached limit, returning last match index
}
bool StrRep::operator==(const StrRep& other) const {  //content comparison
  uint16_t lenT = strLen();
  {
    uint16_t lenO = other.strLen();
    if (lenT != lenO) return false;
  }
  auto iT = iteratorForwardV();
  auto iO = other.iteratorForwardV();
  auto& it = *iT;
  auto& io = *iO;
  while (it.notEnd())
}
StrSpan StrRep::getSpan(uint16_t from, uint16_t until) const {
  return StrSpan(*this, from, until);
}
SizeInfo StrRep::sizeInfo() {
  SizeInfo info;
  auto itp = iteratorForwardV();
  auto& it = *itp;
  while (it.notEnd()) {
    if (*it == '\0') goto lenFound;
    ++info.size;
    ++it;
  }
  info.len = info.size;
  goto end;
lenFound:
  info.len = info.size;
  while (it.notEnd()) {
    ++info.size;
    ++it;
  }
end:
  return info;
}
SPtr<StrRep> StrRep::getProxyLen() {
  return SPtr<StrRep>(new LenSizeProxy(*this));
}
char StrEmpty::readAt(uint16_t index) {
  return '\0';
}
uint16_t StrEmpty::getSize() {
  return 0;
}
StrVar& StrVar::truncate(uint16_t lengthLeft = 0) {
  if (lengthLeft < getSize())
    iteamAt(lengthLeft) = '\0';
  return *this;
}
StrVar& StrVar::fillWith(const StrRep& rep) {
  uint16_t limit = getSize();         //my size
  if (uLimited(limit, rep.strLen()))  //if other shorter
    iteamAt(limit) = '\0';            //write endl
  auto itp = rep.iteratorForwardV();
  auto& it = *itp;
  uint16_t i = 0;
  while (i < limit) {
    iteamAt(i) = *it;
    ++it;
    ++i;
  }
  return *this;
}
StrVar& StrVar::reverse() {
  uint16_t end = strLen();  //size of string
  if (!end) goto end;       //if empty do nothing
  --end;                    //end is last index
  uint16_t beg = 0;         //beg is first index
  while (beg < end) {       //swap chars around middle until pointing to the same or passed over each other
    swap(itemAt(beg), itemAt(end));
    ++beg;
    --end;
  }
end:
  return *this;
}
StrVar& StrVar::paste(const StrRep& rep, uint16_t from, uint16_t until) {
  auto inf = sizeInfo();
  uLimited(until, inf.size);  //indexes limited by size
  uLimited(from, until);
  if (from > inf.len) goto end;                 //if pasted not attached to string body
  if (inSpanRStrict(inf.len, until, inf.size))  //if not leaving \0 behind AND until inside (\0; size)
    itemAt(until) = '\0';
  for (; from < until; ++from)  //for indexes from from until until write sample
    itemAt(from) = rep.readAt(from);
end:
  return *this;
}
StrVar& StrVar::append(const StrRep& rep) {
  uint16_t i;      //this index
  uint16_t j = 0;  //other index
  uint16_t jLimit;
  {
    auto inf = sizeInfo();
    i = inf.len;
    jLimit = min(rep.strLen(), inf.size - i);
  }
  while (j < jLimit) {
    itemAt(i) = rep.readAt(j);
    ++i;
    ++j;
  }
  return *this;
}
StrVar& StrVar::replace(const StrRep& instances, const StrRep& with, uint16_t from = 0; uint16_t numberLimit = 0xFFFF) {
  uint16_t instanceLen = instance.strLen();
  while (numberLimit) {
    from = indexOf(instances, from);
    if (from == 0xFFFF) break;
    removeSpan(from, from + instanceLen);
    insertAt(with, from);
    --numberLimit;
  }
  return *this;
}
StrVar& StrVar::replaceBack(const StrRep& instances, const StrRep& with, uint16_t backFrom = 0xFFFF; uint16_t numberLimit = 0xFFFF) {
  uint16_t instanceLen = instance.strLen();
  while (numberLimit) {
    backFrom = indexOfLast(instances, backFrom);
    if (from == 0xFFFF) break;
    removeSpan(backFrom, backFrom + instanceLen);
    insertAt(with, backFrom);
    --numberLimit;
  }
  return *this;
}
StrVar& StrVar::insertAt(const StrRep& sample, uint16_t index) {
  auto inf = sizeInfo();
  uLimited(index, inf.len);                                         //index not further than len
  uint16_t oLen = sample.strLen();                                  //len of sample, otherLength
  uint16_t pasteLimit = uLimit(index + sample.strLen(), inf.size);  //index after last character of sample in this string
  //check existence of tail to shift
  if (index < inf.len) {                                                   //tail exist(including \0)
    uint16_t tailLimit = index - 1;                                        //tail limit right before index (even if 0xFFFF)
    uint16_t tailSrc =                                                     //index from which start copying tail
      uLimitStrict<uint16_t>(inf.len, inf.size - uLimit(oLen, inf.size));  //oLen sets limit to character that won't disappear
    uint16_t tailDest = tailSrc + oLen;
    while (tailSrc != tailLimit) {         //while not reached limit
      itemAt(tailDest) = readAt(tailSrc);  //copy char
      --tailDest;                          //shift left
      --tailSrc;
    }
  } else if (pasteLimit < inf.size)  //no tail(excluding '\0'), if limit inside container place \0 on it
    itemAt(pasteLimit) = '\0';
  {                                        //tail shifted, now pasting sample left to right
    uint16_t src = 0;                      //first index of sample
    while (index < pasteLimit) {           //until limit
      itemAt(index) = sample.readAt(src);  //write chars from sample string
      ++index;                             //increase
      ++src;
    }
  }
  return *this;
}
StrVar& StrVar::insertBack(const StrRep& sample, uint16_t index) {
  s;
}
StrVar& StrVar::removeSpan(uint16_t from, uint16_t until) {  //move span [until, len] to [from]
  auto inf = sizeInfo();
  uLimited(until, inf.size);  //indexes limited by size
  uLimited(from, until);
  if (inf.len <= from) return *this;  //not affecting
  if (until == from) return *this;    //not affecting
  while (until < inf.len) {           //while until not reached end of string
    itemAt(from) = readAt(until);     //rewrite chars
    ++from;
  }
  itemAt(from) = '\0';  //place end
  return *this;
}
StrVal StrVar::cut(uint16_t from, uint16_t until) {  //remove span and return it
  auto inf = sizeInfo();
  uLimited(until, inf.size);  //indexes limited by size
  uLimited(from, until);
  StrVal str(getSpan(from, until));
  removeSpan(from, until);
  return str;
}
StrSpanVar StrVar::getSpanVar(uint16_t from, uint16_t until) const {
  return StrSpanVar(*this, from, until);
}
SPtr<StrVar> StrVar::getProxyVarLen() {
  return SPtr<StrVar>(new LenSizeProxy(*this));
}
uint16_t StrVar::LenSizeProxy::getSize() {
  return strLen();
}