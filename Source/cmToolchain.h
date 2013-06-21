
#ifndef cmToolchain_h
#define cmToolchain_h

#include "cmStandardIncludes.h"

class cmMakefile;
class cmSourceFile;
class cmCacheManager;

class cmToolchain
{
public:
  cmToolchain(cmMakefile const *mf);

  const char *GetDefinition(const char *) const;
  const char *GetSafeDefinition(const char *) const;
  const char* GetRequiredDefinition(const char* name) const;

  bool IsOn(const char* name) const;
  bool IsSet(const char* name) const;

  bool ReadListFile(const char* listfile,
                    const char* external= 0);

  void SetMakefile(cmMakefile const *mf) { this->Makefile = mf; }

  bool GetLanguageEnabled(const char*) const;
  void ClearEnabledLanguages();
  void GetEnabledLanguages(std::vector<std::string>& lang);
  bool IgnoreFile(const char* ext);
  void FillExtensionToLanguageMap(const char* l, cmMakefile* mf);
  void SetLanguageEnabledFlag(const char* l, cmMakefile* mf);
  void SetLanguageEnabledMaps(const char* l, cmMakefile* mf);
  void EnableLanguagesFromToolchain(cmToolchain *tch);
  const char* GetLanguageFromExtension(const char* ext);
  const char* GetLanguageOutputExtension(cmSourceFile const&);
  void SetLanguageEnabled(const char*, cmMakefile* mf);
  int GetLinkerPreference(const char* lang);
  std::string GetSharedLibFlagsForLanguage(std::string const& lang);
private:
  cmMakefile const *Makefile;
  // If you add a new map here, make sure it is copied
  // in EnableLanguagesFromGenerator
public:
  std::map<cmStdString, bool> LanguageEnabled;
  std::map<cmStdString, bool> IgnoreExtensions;
  std::set<cmStdString> LanguagesReady; // Ready for try_compile
  std::map<cmStdString, cmStdString> OutputExtensions;
  std::map<cmStdString, cmStdString> LanguageToOutputExtension;
  std::map<cmStdString, cmStdString> ExtensionToLanguage;
  std::map<cmStdString, int> LanguageToLinkerPreference;
  std::map<cmStdString, cmStdString> LanguageToOriginalSharedLibFlags;
private:
  cmCacheManager *CacheManager;
};

#endif
