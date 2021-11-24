#ifndef COMMAND_H
#define COMMAND_H

#include "snowlight.h"

enum LightCommandType {
    LIGHT_OFF = 0,
    LIGHT_ON,
    LIGHT_SET,
    LIGHT_DIM,
    LIGHT_BRIGHTEN,

    BUTTON0_CLICK,
    BUTTON0_LONGCLICK,

    END_COMMANDS,

    NO_COMMAND = 255  // default
};

extern const char *commandNames[];

struct RawCommand {
    uint8_t field[N_CMD_FIELDS];  // fields of the command
};

String rawCommandToString(RawCommand rawCmd);

class LightCommand {
   public:
    LightCommand();
    LightCommand(RawCommand rawCmd);
    LightCommandType &getType();
    void setType(LightCommandType type);
    bool isEmpty();
    RawCommand getRawCommand();
    String commandTypeToString();
    String toString();
    bool operator==(const LightCommand &b) const;
    uint8_t getField(int id);

   private:
    LightCommandType _type;  // the type of the command
    RawCommand _rawCmd;      // raw command
};

#endif