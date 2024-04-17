#pragma once

#include <vector>

class PolyMidiBuffer {
  std::vector<int> notes;
  std::vector<int> notes_order;
  int counter;
  int size;
public:
  PolyMidiBuffer(int size);
  void addNote(int note_number);
  void removeNote(int note_number);
  int operator[](int index);
};