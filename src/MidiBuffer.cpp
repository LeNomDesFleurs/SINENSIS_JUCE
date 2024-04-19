#include "MidiBuffer.hpp"

PolyMidiBuffer::PolyMidiBuffer(int size) {
  notes.resize(size);
  notes_order.resize(size);
  std::fill(notes.begin(), notes.end(), 0);
  std::fill(notes_order.begin(), notes_order.end(), 0);
  this->size = size;
  counter = 0;
}

void PolyMidiBuffer::addNote(int note_number) {
  // return if note already exist
  for (int i = 0; i < size; i++) {
    if (note_number == notes[i]) {
      return;
    }
  }
  // si le buffer est plein, remplacer la note la plus vieille
  // size is the oldest note, 1 is the newest
  if (counter == size) {
    for (int i = 0; i < size; i++) {
      if (notes_order[i] == size) {
        notes[i] = note_number;
        notes_order[i] = 1;
      } else {
        notes_order[i]++;
      }
    }
  } else {
    // si le buffer à une place de libre : prendre la première qu'il trouve
    for (int i = 0; i < size; i++) {
      if (notes[i] == 0) {
        notes[i] = note_number;
        counter++;
        return;
      }
    }
  }
}

void PolyMidiBuffer::removeNote(int note_number) {
  if (note_number == 0) {
    return;
  }
  for (int i = 0; i < size; i++) {
    if (notes[i] == note_number) {
      notes[i] = 0;
      notes_order[i] = 0;
      counter--;
      return;
    }
  }
}

int PolyMidiBuffer::operator[](int index) { return this->notes[index]; }