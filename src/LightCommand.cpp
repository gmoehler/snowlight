#include "LightCommand.h"

const char* commandNames[] = {"LIGHT_OFF",  "LIGHT_ON",      "LIGHT_TOGGLE",
                              "LIGHT_SET",  "LIGHT_DIM",     "LIGHT_BRIGHTEN",
                              "BUT0_CLICK", "BUT0_LONGCLICK"};

const char* commandNameNoCommand = "NO_COMMAND      ";
const char* commandNameUnknownCmd = "UNKNOWN_COMMAND ";

LightCommand::LightCommand() : _type(NO_COMMAND) {}

LightCommand::LightCommand(RawCommand rawCmd) {
    _rawCmd = rawCmd;

    // determine command type
    uint8_t f0 = _rawCmd.field[0];
    if (f0 < END_COMMANDS) {
        _type = static_cast<LightCommandType>(f0);
    } else if (f0 == 255) {
        _type = NO_COMMAND;
    } else {
        _type = NO_COMMAND;
        LOGE(LIGHTCMD, "Unknown raw command: %s",
             rawCommandToString(rawCmd).c_str());
    }
}

LightCommandType& LightCommand::getType() { return _type; }

void LightCommand::setType(LightCommandType type) { _type = type; }

bool LightCommand::isEmpty() { return (_type == NO_COMMAND); }

RawCommand LightCommand::getRawCommand() { return _rawCmd; }

String LightCommand::commandTypeToString() {
    char buffer[18];
    uint8_t f0 = _rawCmd.field[0];
    switch (_type) {
        case NO_COMMAND:
            snprintf(buffer, 18, "%-16s", commandNameNoCommand);
            break;
        default:
            if (f0 < END_COMMANDS) {
                snprintf(buffer, 18, "%-16s", commandNames[f0]);
            } else {
                snprintf(buffer, 18, "%-16s", commandNameUnknownCmd);
            }
            break;
    }
    return String(buffer);
}

String LightCommand::toString() {
    char buffer[44];
    snprintf(buffer, 44, "%s  %s", commandTypeToString().c_str(),
             rawCommandToString(_rawCmd).c_str());
    return String(buffer);
}

String rawCommandToString(RawCommand rawCmd) {
    char buffer[40];
    snprintf(buffer, 40, "%3d %3d %3d", rawCmd.field[0], rawCmd.field[1],
             rawCmd.field[2]);
    return String(buffer);
}
bool LightCommand::operator==(const LightCommand& b) const {
    for (int i = 0; i < N_CMD_FIELDS; i++) {
        if (_rawCmd.field[i] != b._rawCmd.field[i]) {
            return false;
        }
    }
    return true;
}

uint8_t LightCommand::getField(int idx) {
    if (idx > N_CMD_FIELDS - 1) {
        LOGE(LIGHTCMD, "field %d of cmd %s does not exist", idx,
             toString().c_str());
        return 0;
    }
    return _rawCmd.field[idx];
}
