/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2004-2011 Kitware, Inc.
  Copyright 2011 Alexander Neundorf (neundorf@kde.org)

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/

#ifndef cmQtAutoGenerators_h
#define cmQtAutoGenerators_h

class cmGlobalGenerator;
class cmMakefile;

class cmQtAutoGenerators
{
public:
  cmQtAutoGenerators();
  bool Run(const std::string& targetDirectory, const std::string& config);

  bool InitializeAutogenTarget(cmTarget* target);
  void SetupAutoGenerateTarget(cmTarget const* target);
  void SetupSourceFiles(cmTarget const* target);

private:
  void SetupAutoMocTarget(cmTarget const* target,
                          const std::string &autogenTargetName,
                          std::map<std::string, std::string> &configIncludes,
                          std::map<std::string, std::string> &configDefines);
  void SetupAutoUicTarget(cmTarget const* target,
                        std::map<std::string, std::string> &configUicOptions);
  void SetupAutoRccTarget(cmTarget const* target);
  void SetupAutoDBusTarget(cmTarget const* target);

  bool ReadAutogenInfoFile(cmMakefile* makefile,
                           const std::string& targetDirectory,
                           const std::string& config);
  bool ReadOldMocDefinitionsFile(cmMakefile* makefile,
                                 const std::string& targetDirectory);
  void WriteOldMocDefinitionsFile(const std::string& targetDirectory);

  std::string MakeCompileSettingsString(cmMakefile* makefile);

  bool RunAutogen(cmMakefile* makefile);
  bool GenerateMoc(const std::string& sourceFile,
                   const std::string& mocFileName);
  bool GenerateUi(const std::string& path, const std::string& uiFileName);
  bool GenerateQrc();
  bool GenerateXml2Cpp();
  bool GenerateXml2CppInterfaces(std::map<std::string, std::string> const& ifaces);
  void ParseCppFile(const std::string& absFilename,
                    const std::vector<std::string>& headerExtensions,
                    std::map<std::string, std::string>& includedMocs,
                          std::map<std::string, std::string>& includedUis,
                              std::map<std::string, std::string> &includedInterfaces);
  void StrictParseCppFile(const std::string& absFilename,
                          const std::vector<std::string>& headerExtensions,
                          std::map<std::string, std::string>& includedMocs,
                          std::map<std::string, std::string>& includedUis,
                              std::map<std::string, std::string> &includedInterfaces);
  void SearchHeadersForCppFile(const std::string& absFilename,
                              const std::vector<std::string>& headerExtensions,
                              std::set<std::string>& absHeaders);

  void ParseHeaders(const std::set<std::string>& absHeaders,
                    const std::map<std::string, std::string>& includedMocs,
                    std::map<std::string, std::string>& notIncludedMocs,
                          std::map<std::string, std::string>& includedUis);

  void ParseForUic(const std::string& fileName,
                   const std::string& contentsString,
                   std::map<std::string, std::string>& includedUis);

  void ParseForUic(const std::string& fileName,
                   std::map<std::string, std::string>& includedUis);

  void ParseForDBusUse(const std::string& fileName,
                       const std::string& contentsString,
                       std::map<std::string, std::string>& includedUis);

  void Init();

  std::string Join(const std::vector<std::string>& lst, char separator);
  bool EndsWith(const std::string& str, const std::string& with);
  bool StartsWith(const std::string& str, const std::string& with);

  void MergeUicOptions(std::vector<std::string> &opts,
                       const std::vector<std::string> &fileOpts, bool isQt5);

  void MergeRccOptions(std::vector<std::string> &opts,
                       const std::vector<std::string> &fileOpts, bool isQt5);

  std::string QtMajorVersion;
  std::string Sources;
  std::string RccSources;
  std::string DBusXmlSources;
  std::string SkipMoc;
  std::string SkipUic;
  std::string Headers;
  bool IncludeProjectDirsBefore;
  std::string Srcdir;
  std::string Builddir;
  std::string MocExecutable;
  std::string UicExecutable;
  std::string RccExecutable;
  std::string Xml2CppExecutable;
  std::string MocCompileDefinitionsStr;
  std::string MocIncludesStr;
  std::string MocOptionsStr;
  std::string ProjectBinaryDir;
  std::string ProjectSourceDir;
  std::string TargetName;
  std::vector<std::string> AvailableDBusXmlSources;

  std::string CurrentCompileSettingsStr;
  std::string OldCompileSettingsStr;

  std::string OutMocCppFilename;
  std::list<std::string> MocIncludes;
  std::list<std::string> MocDefinitions;
  std::vector<std::string> MocOptions;
  std::vector<std::string> DBusInterfaces;
  std::vector<std::string> UicTargetOptions;
  std::map<std::string, std::string> UicOptions;
  std::map<std::string, std::string> RccOptions;

  bool Verbose;
  bool ColorOutput;
  bool RunMocFailed;
  bool RunUicFailed;
  bool RunRccFailed;
  bool GenerateAll;
  bool RelaxedMode;
};

class cmQtAutoDBusXmlGenerator
{
public:
  cmQtAutoDBusXmlGenerator();

  static void SetupAutoDBusXmlTarget(std::vector<cmTarget*> targets);

  bool RunDBusCpp2Xml(const char* targetDirectory);

private:
  bool RunAutoDBusCpp2Xml(cmMakefile* makefile);
  bool ReadAutoDBusInfoFile(cmMakefile* makefile,
                            const char* targetDirectory);
  bool GenerateDBusXml(const std::string &executable,
                       const std::map<std::string, std::map<std::string, std::string> >& dbusFiles);

  std::string ProjectBinaryDir;
  std::string ProjectSourceDir;
  std::string Qt4DbusCpp2XmlExecutable;
  std::string Qt5DbusCpp2XmlExecutable;
  std::vector<std::string> Qt4DBusTargets;
  std::vector<std::string> Qt5DBusTargets;
  std::map<std::string, std::string> DBusTargetSources;
  std::map<std::string, std::vector<std::string> > DBusTargetOptions;

  bool Verbose;
  bool RunDBusCpp2XmlFailed;
  bool GenerateAll;
};

#endif
