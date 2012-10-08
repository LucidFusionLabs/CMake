/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2012 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef cmGeneratorTarget_h
#define cmGeneratorTarget_h

#include "cmStandardIncludes.h"
#include "cmGeneratorExpression.h"

class cmComputeLinkInformation;
class cmCustomCommand;
class cmGlobalGenerator;
class cmLocalGenerator;
class cmMakefile;
class cmSourceFile;
class cmTarget;

// Basic information about each link item.
class cmLinkItem: public std::string
{
  typedef std::string std_string;
public:
  cmLinkItem(): std_string(), Target(0) {}
  cmLinkItem(const std_string& n,
             cmTarget const* t): std_string(n), Target(t) {}
  cmLinkItem(cmLinkItem const& r): std_string(r), Target(r.Target) {}
  cmTarget const* Target;
};
class cmLinkImplItem: public cmLinkItem
{
public:
  cmLinkImplItem(): cmLinkItem(), Backtrace(0), FromGenex(false) {}
  cmLinkImplItem(std::string const& n,
                 cmTarget const* t,
                 cmListFileBacktrace const& bt,
                 bool fromGenex):
    cmLinkItem(n, t), Backtrace(bt), FromGenex(fromGenex) {}
  cmLinkImplItem(cmLinkImplItem const& r):
    cmLinkItem(r), Backtrace(r.Backtrace), FromGenex(r.FromGenex) {}
  cmListFileBacktrace Backtrace;
  bool FromGenex;
};

struct cmTargetLinkInformationMap:
  public std::map<std::pair<cmTarget const*, std::string>,
                           cmComputeLinkInformation*>
{
  typedef std::map<std::pair<cmTarget const*, std::string>,
                   cmComputeLinkInformation*> derived;
  cmTargetLinkInformationMap() {}
  cmTargetLinkInformationMap(cmTargetLinkInformationMap const& r);
  ~cmTargetLinkInformationMap();
};

class cmGeneratorTarget
{
public:
  cmGeneratorTarget(cmTarget*);
  ~cmGeneratorTarget();

  bool IsImported() const;
  const char *GetLocation(const std::string& config) const;

  /** Get the location of the target in the build tree with a placeholder
      referencing the configuration in the native build system.  This
      location is suitable for use as the LOCATION target property.  */
  const char* GetLocationForBuild() const;

  int GetType() const;
  std::string GetName() const;
  const char *GetProperty(const std::string& prop) const;
  bool GetPropertyAsBool(const std::string& prop) const;
  void GetSourceFiles(std::vector<cmSourceFile*>& files,
                      const std::string& config) const;

  void GetObjectSources(std::vector<cmSourceFile const*> &,
                        const std::string& config) const;
  const std::string& GetObjectName(cmSourceFile const* file);

  bool HasExplicitObjectName(cmSourceFile const* file) const;
  void AddExplicitObjectName(cmSourceFile const* sf);

  void GetResxSources(std::vector<cmSourceFile const*>&,
                      const std::string& config) const;
  void GetIDLSources(std::vector<cmSourceFile const*>&,
                     const std::string& config) const;
  void GetExternalObjects(std::vector<cmSourceFile const*>&,
                          const std::string& config) const;
  void GetHeaderSources(std::vector<cmSourceFile const*>&,
                        const std::string& config) const;
  void GetExtraSources(std::vector<cmSourceFile const*>&,
                       const std::string& config) const;
  void GetCustomCommands(std::vector<cmSourceFile const*>&,
                         const std::string& config) const;
  void GetExpectedResxHeaders(std::set<std::string>&,
                              const std::string& config) const;
  void GetAppManifest(std::vector<cmSourceFile const*>&,
                      const std::string& config) const;
  void GetCertificates(std::vector<cmSourceFile const*>&,
                       const std::string& config) const;

  void ComputeObjectMapping();

  /** Get the full path to the target according to the settings in its
      makefile and the configuration type.  */
  std::string GetFullPath(const std::string& config="", bool implib = false,
                          bool realname = false) const;
  std::string NormalGetFullPath(const std::string& config, bool implib,
                                bool realname) const;
  std::string NormalGetRealName(const std::string& config) const;

  /** Get the soname of the target.  Allowed only for a shared library.  */
  std::string GetSOName(const std::string& config) const;

  void GetFullNameComponents(std::string& prefix,
                             std::string& base, std::string& suffix,
                             const std::string& config="",
                             bool implib = false) const;

  cmTarget* Target;
  cmMakefile* Makefile;
  cmLocalGenerator* LocalGenerator;
  cmGlobalGenerator const* GlobalGenerator;

  std::string GetModuleDefinitionFile(const std::string& config) const;

  /** Full path with trailing slash to the top-level directory
      holding object files for this target.  Includes the build
      time config name placeholder if needed for the generator.  */
  std::string ObjectDirectory;

  void UseObjectLibraries(std::vector<std::string>& objs,
                          const std::string& config) const;

  mutable cmTargetLinkInformationMap LinkInformation;

  cmComputeLinkInformation* GetLinkInformation(const std::string& config,
                                               cmTarget const* head = 0) const;

  void GetTransitiveTargetClosure(const std::string& config,
                                        cmTarget const* headTarget,
                                        std::vector<cmTarget*> &libs) const;

  bool IsLinkInterfaceDependentBoolProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentStringProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentNumberMinProperty(const std::string& p,
                                            const std::string& config) const;
  bool IsLinkInterfaceDependentNumberMaxProperty(const std::string& p,
                                            const std::string& config) const;

  bool GetLinkInterfaceDependentBoolProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentStringProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentNumberMinProperty(const std::string& p,
                                            const std::string& config) const;
  const char *GetLinkInterfaceDependentNumberMaxProperty(const std::string& p,
                                            const std::string& config) const;

  void CheckPropertyCompatibility(cmComputeLinkInformation *info,
                                  const std::string& config) const;

  cmMakefile* GetMakefile() const;

  void GetAppleArchs(const std::string& config,
                     std::vector<std::string>& archVec) const;

  /** Return the rule variable used to create this type of target.  */
  std::string GetCreateRuleVariable(std::string const& lang,
                                    std::string const& config) const;

  /** Get the include directories for this target.  */
  std::vector<std::string> GetIncludeDirectories(
      const std::string& config) const;

  bool IsSystemIncludeDirectory(const std::string& dir,
                                const std::string& config) const;

  /** Add the target output files to the global generator manifest.  */
  void GenerateTargetManifest(const std::string& config) const;

  /**
   * Trace through the source files in this target and add al source files
   * that they depend on, used by all generators
   */
  void TraceDependencies();

  /** Get sources that must be built before the given source.  */
  std::vector<cmSourceFile*> const*
  GetSourceDepends(cmSourceFile const* sf) const;

  /** Get the name of the pdb file for the target.  */
  std::string GetPDBName(const std::string& config="") const;

  /** Whether this library has soname enabled and platform supports it.  */
  bool HasSOName(const std::string& config) const;

  /** Get the name of the compiler pdb file for the target.  */
  std::string GetCompilePDBName(const std::string& config="") const;

  /** Get the path for the MSVC /Fd option for this target.  */
  std::string GetCompilePDBPath(const std::string& config="") const;

  class TargetPropertyEntry {
    static cmLinkImplItem NoLinkImplItem;
  public:
    TargetPropertyEntry(cmsys::auto_ptr<cmCompiledGeneratorExpression> cge,
                        cmLinkImplItem const& item = NoLinkImplItem)
      : ge(cge), LinkImplItem(item)
    {}
    const cmsys::auto_ptr<cmCompiledGeneratorExpression> ge;
    cmLinkImplItem const& LinkImplItem;
  };

  /**
   * Flags for a given source file as used in this target. Typically assigned
   * via SET_TARGET_PROPERTIES when the property is a list of source files.
   */
  enum SourceFileType
  {
    SourceFileTypeNormal,
    SourceFileTypePrivateHeader, // is in "PRIVATE_HEADER" target property
    SourceFileTypePublicHeader,  // is in "PUBLIC_HEADER" target property
    SourceFileTypeResource,      // is in "RESOURCE" target property *or*
                                 // has MACOSX_PACKAGE_LOCATION=="Resources"
    SourceFileTypeMacContent     // has MACOSX_PACKAGE_LOCATION!="Resources"
  };
  struct SourceFileFlags
  {
    SourceFileFlags(): Type(SourceFileTypeNormal), MacFolder(0) {}
    SourceFileFlags(SourceFileFlags const& r):
      Type(r.Type), MacFolder(r.MacFolder) {}
    SourceFileType Type;
    const char* MacFolder; // location inside Mac content folders
  };
  void GetAutoUicOptions(std::vector<std::string> &result,
                         const std::string& config) const;

  void ReportPropertyOrigin(const std::string &p,
                            const std::string &result,
                            const std::string &report,
                            const std::string &compatibilityType) const;


  /** Get the names of the executable needed to generate a build rule
      that takes into account executable version numbers.  This should
      be called only on an executable target.  */
  void GetExecutableNames(std::string& name, std::string& realName,
                          std::string& impName, std::string& pdbName,
                          const std::string& config) const;

  /** Get the names of the library needed to generate a build rule
      that takes into account shared library version numbers.  This
      should be called only on a library target.  */
  void GetLibraryNames(std::string& name, std::string& soName,
                       std::string& realName, std::string& impName,
                       std::string& pdbName, const std::string& config) const;

  /**
   * Compute whether this target must be relinked before installing.
   */
  bool NeedRelinkBeforeInstall(const std::string& config) const;

  /** Return true if builtin chrpath will work for this target */
  bool IsChrpathUsed(const std::string& config) const;

  struct SourceFileFlags
  GetTargetSourceFileFlags(const cmSourceFile* sf) const;

  struct ResxData {
    mutable std::set<std::string> ExpectedResxHeaders;
    mutable std::vector<cmSourceFile const*> ResxSources;
  };
private:
  friend class cmTargetTraceDependencies;
  struct SourceEntry { std::vector<cmSourceFile*> Depends; };
  typedef std::map<cmSourceFile const*, SourceEntry> SourceEntriesType;
  SourceEntriesType SourceEntries;
  mutable std::map<cmSourceFile const*, std::string> Objects;
  std::set<cmSourceFile const*> ExplicitObjectName;
  std::set<std::string> ExpectedResxHeaders;
  mutable std::map<std::string, std::vector<std::string> > SystemIncludesCache;

  void ConstructSourceFileFlags() const;
  mutable bool SourceFileFlagsConstructed;
  mutable std::map<cmSourceFile const*, SourceFileFlags> SourceFlagsMap;
  mutable bool DebugIncludesDone;
  mutable std::map<std::string, std::vector<TargetPropertyEntry*> >
                                CachedLinkInterfaceIncludeDirectoriesEntries;
  mutable std::map<std::string, bool> CacheLinkInterfaceIncludeDirectoriesDone;
  mutable std::map<std::string, bool> DebugCompatiblePropertiesDone;

  cmGeneratorTarget(cmGeneratorTarget const&);
  void operator=(cmGeneratorTarget const&);

  struct CompatibleInterfaces
  {
    std::set<std::string> PropsBool;
    std::set<std::string> PropsString;
    std::set<std::string> PropsNumberMax;
    std::set<std::string> PropsNumberMin;
  };
  CompatibleInterfaces const&
    GetCompatibleInterfaces(std::string const& config) const;

  struct CompatibleInterfacesIntl: CompatibleInterfaces
  {
    CompatibleInterfacesIntl(): Done(false) {}
    bool Done;
  };
  mutable std::map<std::string, CompatibleInterfacesIntl> CompatibleInterfacesMap;

  struct LinkImplClosure: public std::vector<cmTarget const*>
  {
    LinkImplClosure(): Done(false) {}
    bool Done;
  };
  mutable std::map<std::string, LinkImplClosure> LinkImplClosureMap;

public:
  std::vector<cmTarget const*> const&
    GetLinkImplementationClosure(const std::string& config) const;

};

struct cmStrictTargetComparison {
  bool operator()(cmTarget const* t1, cmTarget const* t2) const;
};

typedef std::map<cmTarget const*,
                 cmGeneratorTarget*,
                 cmStrictTargetComparison> cmGeneratorTargetsType;

#endif
