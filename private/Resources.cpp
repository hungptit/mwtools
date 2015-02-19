template <> const std::string Resources<std::string>::CVSFolder = Tools::FileSeparator<std::string>::value + "CVS" + Tools::FileSeparator<std::string>::value;
template <> const std::string Resources<std::string>::DerivedFolder = Tools::FileSeparator<std::string>::value + "derived" + Tools::FileSeparator<std::string>::value;
template <> const std::string Resources<std::string>::SbtoolsFolder = ".sbtools";
template <> const std::string Resources<std::string>::DatabaseFileName = "database.txt";
template <> const std::string Resources<std::string>::NetworkSandbox = Tools::getEnvironmentVariableValue("DEFAULT_SANDBOX");
template <> const std::string Resources<std::string>::BackupDirectory = Resources<std::string>::NetworkSandbox + Tools::FileSeparator<std::string>::value + "backup";
template <> const std::string Resources<std::string>::LatestSnapshotName = "@latest";
template <> const std::string Resources<std::string>::ReviewBoardCommand = "sbreviewboard -cn ";
template <> const std::string Resources<std::string>::SbCommand = "sb ";
template <> const std::string Resources<std::string>::SbCheckCommand = "sbcheck ";

template <> const std::string Resources<std::string>::WebViewer = "chromium";
template <> const std::string Resources<std::string>::Gecko = "http://komodo.mathworks.com/main/gecko/";
template <> const std::string Resources<std::string>::GeckoSearch = "http://inside-labs.mathworks.com/dev/gecko-search/?q=";
template <> const std::string Resources<std::string>::Batcave = "http://bat3cave.mathworks.com/Cabbage/cluster/";
template <> const std::string Resources<std::string>::CodeSearch = "http://codesearch.mathworks.com:8080/srcsearch/";
template <> const std::string Resources<std::string>::Outlook = "https://outlook.mathworks.com/owa/#path=/";
template <> const std::string Resources<std::string>::InsidePage = "http://inside.mathworks.com/";
template <> const std::string Resources<std::string>::WikiPage = "http://inside.mathworks.com/wiki/Main_Page";
template <> const std::string Resources<std::string>::Google = "https://www.google.com/?gws_rd=ssl";
template <> const std::string Resources<std::string>::ISOCppPage = "http://isocpp.org/";
template <> const std::string Resources<std::string>::AtTaskPage = "https://mathworks.attask-ondemand.com/myWork";
template <> const std::string Resources<std::string>::Perforce = "p4";
template <> const std::string Resources<std::string>::PerforceDatabase = "http://p4dbmaster.mathworks.com/cgi-bin/";






