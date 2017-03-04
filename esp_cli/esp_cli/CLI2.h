/*
 Command-line interface wrapper for ESP8266-Arduino.
 
 Distributed under MIT license.

 By Alexander Voronin <av1024@gmail.com>, 2015
*/
// CLI v2

#pragma once

namespace CLI {

// max supported commands
#define CLI_MAX_COMMANDS 20
// max arguments including command name
#define CLI_MAX_PARAMS  5


class Command;
class Handler;

typedef bool (cmdHandler) (Handler&, Print&);

class Command {
  protected:
    String _name;
    String _short;
    String _long;
    cmdHandler *_cb;

    friend Handler;
  public:
    Command();
    Command(String name, String short_help, String long_help, cmdHandler *callback);
    Command(String name, const __FlashStringHelper* short_help, const __FlashStringHelper* long_help, cmdHandler *callback);
};

class Handler {
  private:
    Command _cmds[CLI_MAX_COMMANDS];
    String _argv[CLI_MAX_PARAMS];
    int _argc = 0;
    
  public:
    Handler();
    // add command handler to list
    bool add(String name, String short_help, String long_help, cmdHandler *callback);
    // find index of command
    int find(String name);
    // find and call handler for command, returns handler result
    bool handle(Print &prn, String cmdline);
    
    // get argument by `index`
    String argv(int index) { return ((index<_argc) ? _argv[index] : ""); };
    // get argument count (including command)
    int argc() { return _argc; };
    
    // join arguments back starting from `index`
    String join_from(int index);
    
    // print list of commands or help for passed `cmd`
    void help(Print &prn, String cmd="");
};

} // namespace

