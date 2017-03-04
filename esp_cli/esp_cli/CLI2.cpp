// CLI v2
// TODO: move CLI class to library

#include <Arduino.h>

#include "CLI2.h"

/*
#undef COMPONENT
#undef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#define COMPONENT "CLI2"
#include <trace.h>
*/
#define TRACE(...) { ; }

namespace CLI {

Handler::Handler() {
  ; //
}

int Handler::find(String name) {
  if (name=="") return -1;
  for(int i=0;i<CLI_MAX_COMMANDS;i++) {
    if (_cmds[i]._name.equals(name)) return i;
  }
  return -1;
}

bool Handler::add(String name, String short_help, String long_help, cmdHandler *callback) {
  if (find(name) >=0 ) return false;
  for(int i=0;i<CLI_MAX_COMMANDS;i++) {
    if (_cmds[i]._name=="") {
      _cmds[i] = Command(name, short_help, long_help, callback);

      Serial.print(F(" * Register callback for '"));
      Serial.print(name);
      Serial.println("'");
      return i;
    }
  }
  return -1;
}

bool Handler::handle(Print &prn, String cmdline) {
    // split command
    // find in attached commands
    // invoke handler with self
        
    cmdline.trim();
    if (cmdline=="") return false;

    // clear prev result
    _argc = 0;
    for(int i=0;i<CLI_MAX_PARAMS;i++) {
      _argv[i] = "";
    }
    
    // split string by spaces
    for (unsigned int i=0;i<cmdline.length();i++) {
      char c = cmdline.charAt(i);
      if (_argc < CLI_MAX_PARAMS) {
        if (c==' ' || c=='\t') {
          TRACE("argv[%d] @%d = '%s'. Next.", _argc, i, _argv[_argc].c_str());
          _argc++;
          
        } else {
          TRACE("+ char '%c'", c);
          _argv[_argc] += c;
        }
      } else {
        TRACE("REST");
        _argv[_argc-1] += " ";
        _argv[_argc-1] += cmdline.substring(i);
        break;
      }
    }
    if (_argc < CLI_MAX_PARAMS) _argc++;

    if (_argv[0]=="help" || _argv[0]=="?") {
      if (_argc>1) help(prn, _argv[1]);
      else help(prn);
      return true;
    }

    int x;
    if ((x=find(_argv[0]))>=0) {
        Command cmd = _cmds[x];
        return cmd._cb(*this, prn);
        //return (*(_cmds[x]._cb))(this, printer);
    }
    return false;
}


void Handler::help(Print &prn, String cmd) {
  prn.println(F(" = HELP ="));
  
  if (cmd == "") {
   prn.println(F("Try 'help cmd' for detailed help\n\r"));
   prn.println(F("Allowed commands:"));
   
    for(int i=0;i<CLI_MAX_COMMANDS;i++) {
      if (_cmds[i]._name != "") {
        prn.print(_cmds[i]._name);
        prn.print("\t\t");
        prn.println(_cmds[i]._short);
      }
    }
    return;
  }
  
  int i = find(cmd);
  if (i < 0) {
    prn.print(F(" ! No help for '"));
    prn.print(cmd);
    prn.println("'");
    prn.println("Try 'help'");
    return;
  } else {
    prn.print(cmd);
    prn.print("\t\t");
    prn.println(_cmds[i]._short);
    prn.print("\t");
    prn.println(_cmds[i]._long);
  }
}

String Handler::join_from(int index) {
  String s = "";
  for(int i=index;i<_argc;i++) {
    String _s0 = _argv[i];
    TRACE(" +join #%d '%s'", i, _s0.length() ? _s0.c_str() : "NULL");
    if (i != index) s += " ";
    s += _argv[i];
  }
  return s;
}

// ====

Command::Command() {
  ;
}

Command::Command(String name, String short_help, String long_help, cmdHandler * callback) {
  _name = name;
  _short = short_help;
  _long = long_help;
  _cb = callback;
}

Command::Command(String name, const __FlashStringHelper* short_help, const __FlashStringHelper* long_help, cmdHandler * callback) {
  _name = String(name);
  _short = String(short_help);
  _long = String(long_help);
  _cb = callback;
}


} // namespace


