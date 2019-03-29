//===-- ScriptInterpreterPython.h -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_PLUGINS_SCRIPTINTERPRETER_PYTHON_SCRIPTINTERPRETERPYTHON_H
#define LLDB_PLUGINS_SCRIPTINTERPRETER_PYTHON_SCRIPTINTERPRETERPYTHON_H

#ifdef LLDB_DISABLE_PYTHON

// Python is disabled in this build

#else

#include <memory>
#include <string>
#include <vector>

#include "PythonDataObjects.h"
#include "lldb/Breakpoint/BreakpointOptions.h"
#include "lldb/Core/IOHandler.h"
#include "lldb/Host/Terminal.h"
#include "lldb/Interpreter/ScriptInterpreter.h"
#include "lldb/lldb-private.h"

class IOHandlerPythonInterpreter;

namespace lldb_private {

class ScriptInterpreterPython : public ScriptInterpreter,
                                public IOHandlerDelegateMultiline {
public:
  class CommandDataPython : public BreakpointOptions::CommandData {
  public:
    CommandDataPython() : BreakpointOptions::CommandData() {
      interpreter = lldb::eScriptLanguagePython;
    }
  };

  friend class ::IOHandlerPythonInterpreter;

  ScriptInterpreterPython(CommandInterpreter &interpreter);

  ~ScriptInterpreterPython() override;

  bool Interrupt() override;

  bool ExecuteOneLine(
      llvm::StringRef command, CommandReturnObject *result,
      const ExecuteScriptOptions &options = ExecuteScriptOptions()) override;

  void ExecuteInterpreterLoop() override;

  bool ExecuteOneLineWithReturn(
      llvm::StringRef in_string,
      ScriptInterpreter::ScriptReturnType return_type, void *ret_value,
      const ExecuteScriptOptions &options = ExecuteScriptOptions()) override;

  lldb_private::Status ExecuteMultipleLines(
      const char *in_string,
      const ExecuteScriptOptions &options = ExecuteScriptOptions()) override;

  Status
  ExportFunctionDefinitionToInterpreter(StringList &function_def) override;

  bool GenerateTypeScriptFunction(StringList &input, std::string &output,
                                  const void *name_token = nullptr) override;

  bool GenerateTypeSynthClass(StringList &input, std::string &output,
                              const void *name_token = nullptr) override;

  bool GenerateTypeSynthClass(const char *oneliner, std::string &output,
                              const void *name_token = nullptr) override;

  // use this if the function code is just a one-liner script
  bool GenerateTypeScriptFunction(const char *oneliner, std::string &output,
                                  const void *name_token = nullptr) override;

  bool GenerateScriptAliasFunction(StringList &input,
                                   std::string &output) override;

  StructuredData::ObjectSP
  CreateSyntheticScriptedProvider(const char *class_name,
                                  lldb::ValueObjectSP valobj) override;

  StructuredData::GenericSP
  CreateScriptCommandObject(const char *class_name) override;

  StructuredData::ObjectSP
  CreateScriptedThreadPlan(const char *class_name,
                           lldb::ThreadPlanSP thread_plan) override;

  bool ScriptedThreadPlanExplainsStop(StructuredData::ObjectSP implementor_sp,
                                      Event *event,
                                      bool &script_error) override;

  bool ScriptedThreadPlanShouldStop(StructuredData::ObjectSP implementor_sp,
                                    Event *event, bool &script_error) override;

  bool ScriptedThreadPlanIsStale(StructuredData::ObjectSP implementor_sp,
                                 bool &script_error) override;

  lldb::StateType
  ScriptedThreadPlanGetRunState(StructuredData::ObjectSP implementor_sp,
                                bool &script_error) override;

  StructuredData::GenericSP
  CreateScriptedBreakpointResolver(const char *class_name,
                                   StructuredDataImpl *args_data,
                                   lldb::BreakpointSP &bkpt_sp) override;
  bool
  ScriptedBreakpointResolverSearchCallback(StructuredData::GenericSP
                                               implementor_sp,
                                           SymbolContext *sym_ctx) override;

  lldb::SearchDepth
  ScriptedBreakpointResolverSearchDepth(StructuredData::GenericSP
                                            implementor_sp) override;

  StructuredData::GenericSP
  CreateFrameRecognizer(const char *class_name) override;

  lldb::ValueObjectListSP
  GetRecognizedArguments(const StructuredData::ObjectSP &implementor,
                         lldb::StackFrameSP frame_sp) override;

  StructuredData::GenericSP
  OSPlugin_CreatePluginObject(const char *class_name,
                              lldb::ProcessSP process_sp) override;

  StructuredData::DictionarySP
  OSPlugin_RegisterInfo(StructuredData::ObjectSP os_plugin_object_sp) override;

  StructuredData::ArraySP
  OSPlugin_ThreadsInfo(StructuredData::ObjectSP os_plugin_object_sp) override;

  StructuredData::StringSP
  OSPlugin_RegisterContextData(StructuredData::ObjectSP os_plugin_object_sp,
                               lldb::tid_t thread_id) override;

  StructuredData::DictionarySP
  OSPlugin_CreateThread(StructuredData::ObjectSP os_plugin_object_sp,
                        lldb::tid_t tid, lldb::addr_t context) override;

  StructuredData::ObjectSP
  LoadPluginModule(const FileSpec &file_spec,
                   lldb_private::Status &error) override;

  StructuredData::DictionarySP
  GetDynamicSettings(StructuredData::ObjectSP plugin_module_sp, Target *target,
                     const char *setting_name,
                     lldb_private::Status &error) override;

  size_t CalculateNumChildren(const StructuredData::ObjectSP &implementor,
                              uint32_t max) override;

  lldb::ValueObjectSP
  GetChildAtIndex(const StructuredData::ObjectSP &implementor,
                  uint32_t idx) override;

  int GetIndexOfChildWithName(const StructuredData::ObjectSP &implementor,
                              const char *child_name) override;

  bool UpdateSynthProviderInstance(
      const StructuredData::ObjectSP &implementor) override;

  bool MightHaveChildrenSynthProviderInstance(
      const StructuredData::ObjectSP &implementor) override;

  lldb::ValueObjectSP
  GetSyntheticValue(const StructuredData::ObjectSP &implementor) override;

  ConstString
  GetSyntheticTypeName(const StructuredData::ObjectSP &implementor) override;

  bool
  RunScriptBasedCommand(const char *impl_function, llvm::StringRef args,
                        ScriptedCommandSynchronicity synchronicity,
                        lldb_private::CommandReturnObject &cmd_retobj,
                        Status &error,
                        const lldb_private::ExecutionContext &exe_ctx) override;

  bool RunScriptBasedCommand(
      StructuredData::GenericSP impl_obj_sp, llvm::StringRef args,
      ScriptedCommandSynchronicity synchronicity,
      lldb_private::CommandReturnObject &cmd_retobj, Status &error,
      const lldb_private::ExecutionContext &exe_ctx) override;

  Status GenerateFunction(const char *signature,
                          const StringList &input) override;

  Status GenerateBreakpointCommandCallbackData(StringList &input,
                                               std::string &output) override;

  bool GenerateWatchpointCommandCallbackData(StringList &input,
                                             std::string &output) override;

  static bool BreakpointCallbackFunction(void *baton,
                                         StoppointCallbackContext *context,
                                         lldb::user_id_t break_id,
                                         lldb::user_id_t break_loc_id);

  static bool WatchpointCallbackFunction(void *baton,
                                         StoppointCallbackContext *context,
                                         lldb::user_id_t watch_id);

  bool GetScriptedSummary(const char *function_name, lldb::ValueObjectSP valobj,
                          StructuredData::ObjectSP &callee_wrapper_sp,
                          const TypeSummaryOptions &options,
                          std::string &retval) override;

  void Clear() override;

  bool GetDocumentationForItem(const char *item, std::string &dest) override;

  bool GetShortHelpForCommandObject(StructuredData::GenericSP cmd_obj_sp,
                                    std::string &dest) override;

  uint32_t
  GetFlagsForCommandObject(StructuredData::GenericSP cmd_obj_sp) override;

  bool GetLongHelpForCommandObject(StructuredData::GenericSP cmd_obj_sp,
                                   std::string &dest) override;

  bool CheckObjectExists(const char *name) override {
    if (!name || !name[0])
      return false;
    std::string temp;
    return GetDocumentationForItem(name, temp);
  }

  bool RunScriptFormatKeyword(const char *impl_function, Process *process,
                              std::string &output, Status &error) override;

  bool RunScriptFormatKeyword(const char *impl_function, Thread *thread,
                              std::string &output, Status &error) override;

  bool RunScriptFormatKeyword(const char *impl_function, Target *target,
                              std::string &output, Status &error) override;

  bool RunScriptFormatKeyword(const char *impl_function, StackFrame *frame,
                              std::string &output, Status &error) override;

  bool RunScriptFormatKeyword(const char *impl_function, ValueObject *value,
                              std::string &output, Status &error) override;

  bool
  LoadScriptingModule(const char *filename, bool can_reload, bool init_session,
                      lldb_private::Status &error,
                      StructuredData::ObjectSP *module_sp = nullptr) override;

  bool IsReservedWord(const char *word) override;

  std::unique_ptr<ScriptInterpreterLocker> AcquireInterpreterLock() override;

  void CollectDataForBreakpointCommandCallback(
      std::vector<BreakpointOptions *> &bp_options_vec,
      CommandReturnObject &result) override;

  void
  CollectDataForWatchpointCommandCallback(WatchpointOptions *wp_options,
                                          CommandReturnObject &result) override;

  /// Set the callback body text into the callback for the breakpoint.
  Status SetBreakpointCommandCallback(BreakpointOptions *bp_options,
                                      const char *callback_body) override;

  void SetBreakpointCommandCallbackFunction(BreakpointOptions *bp_options,
                                            const char *function_name) override;

  /// This one is for deserialization:
  Status SetBreakpointCommandCallback(
      BreakpointOptions *bp_options,
      std::unique_ptr<BreakpointOptions::CommandData> &data_up) override;

  /// Set a one-liner as the callback for the watchpoint.
  void SetWatchpointCommandCallback(WatchpointOptions *wp_options,
                                    const char *oneliner) override;

  StringList ReadCommandInputFromUser(FILE *in_file);

  void ResetOutputFileHandle(FILE *new_fh) override;

  const char *GetDictionaryName() { return m_dictionary_name.c_str(); }

  PyThreadState *GetThreadState() { return m_command_thread_state; }

  void SetThreadState(PyThreadState *s) {
    if (s)
      m_command_thread_state = s;
  }

  //----------------------------------------------------------------------
  // IOHandlerDelegate
  //----------------------------------------------------------------------
  void IOHandlerActivated(IOHandler &io_handler, bool interactive) override;

  void IOHandlerInputComplete(IOHandler &io_handler,
                              std::string &data) override;

  //------------------------------------------------------------------
  // Static Functions
  //------------------------------------------------------------------
  static void Initialize();

  static void Terminate();

  static lldb::ScriptInterpreterSP
  CreateInstance(CommandInterpreter &interpreter);

  static lldb_private::ConstString GetPluginNameStatic();

  static const char *GetPluginDescriptionStatic();

  static FileSpec GetPythonDir();

  //------------------------------------------------------------------
  // PluginInterface protocol
  //------------------------------------------------------------------
  lldb_private::ConstString GetPluginName() override;

  uint32_t GetPluginVersion() override;

  class Locker : public ScriptInterpreterLocker {
  public:
    enum OnEntry {
      AcquireLock = 0x0001,
      InitSession = 0x0002,
      InitGlobals = 0x0004,
      NoSTDIN = 0x0008
    };

    enum OnLeave {
      FreeLock = 0x0001,
      FreeAcquiredLock = 0x0002, // do not free the lock if we already held it
                                 // when calling constructor
      TearDownSession = 0x0004
    };

    Locker(ScriptInterpreterPython *py_interpreter = nullptr,
           uint16_t on_entry = AcquireLock | InitSession,
           uint16_t on_leave = FreeLock | TearDownSession, FILE *in = nullptr,
           FILE *out = nullptr, FILE *err = nullptr);

    ~Locker() override;

  private:
    bool DoAcquireLock();

    bool DoInitSession(uint16_t on_entry_flags, FILE *in, FILE *out, FILE *err);

    bool DoFreeLock();

    bool DoTearDownSession();

    static void ReleasePythonLock();

    bool m_teardown_session;
    ScriptInterpreterPython *m_python_interpreter;
    //    	FILE*                    m_tmp_fh;
    PyGILState_STATE m_GILState;
  };

protected:
  static void InitializePrivate();

  class SynchronicityHandler {
  private:
    lldb::DebuggerSP m_debugger_sp;
    ScriptedCommandSynchronicity m_synch_wanted;
    bool m_old_asynch;

  public:
    SynchronicityHandler(lldb::DebuggerSP, ScriptedCommandSynchronicity);

    ~SynchronicityHandler();
  };

  enum class AddLocation { Beginning, End };

  static void AddToSysPath(AddLocation location, std::string path);

  static void ComputePythonDirForApple(llvm::SmallVectorImpl<char> &path);
  static void ComputePythonDirForPosix(llvm::SmallVectorImpl<char> &path);
  static void ComputePythonDirForWindows(llvm::SmallVectorImpl<char> &path);

  bool EnterSession(uint16_t on_entry_flags, FILE *in, FILE *out, FILE *err);

  void LeaveSession();

  void SaveTerminalState(int fd);

  void RestoreTerminalState();

  uint32_t IsExecutingPython() const { return m_lock_count > 0; }

  uint32_t IncrementLockCount() { return ++m_lock_count; }

  uint32_t DecrementLockCount() {
    if (m_lock_count > 0)
      --m_lock_count;
    return m_lock_count;
  }

  enum ActiveIOHandler {
    eIOHandlerNone,
    eIOHandlerBreakpoint,
    eIOHandlerWatchpoint
  };

  PythonObject &GetMainModule();

  PythonDictionary &GetSessionDictionary();

  PythonDictionary &GetSysModuleDictionary();

  bool GetEmbeddedInterpreterModuleObjects();

  bool SetStdHandle(File &file, const char *py_name, PythonFile &save_file,
                    const char *mode);

  PythonFile m_saved_stdin;
  PythonFile m_saved_stdout;
  PythonFile m_saved_stderr;
  PythonObject m_main_module;
  PythonDictionary m_session_dict;
  PythonDictionary m_sys_module_dict;
  PythonObject m_run_one_line_function;
  PythonObject m_run_one_line_str_global;
  std::string m_dictionary_name;
  TerminalState m_terminal_state;
  ActiveIOHandler m_active_io_handler;
  bool m_session_is_active;
  bool m_pty_slave_is_open;
  bool m_valid_session;
  uint32_t m_lock_count;
  PyThreadState *m_command_thread_state;
};

} // namespace lldb_private

#endif // LLDB_DISABLE_PYTHON

#endif // LLDB_PLUGINS_SCRIPTINTERPRETER_PYTHON_SCRIPTINTERPRETERPYTHON_H
