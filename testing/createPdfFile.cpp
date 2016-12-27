#include <string>
#include <iostream>
#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "utils/Utils.hpp"
#include "utils/Process.hpp"

namespace {
    using boost::filesystem::path;

    const std::vector<std::tuple<path, path>> generate(std::vector<std::string> &inputFiles) {
        std::vector<std::tuple<path, path>> info;
        const path rootPath(std::getenv("HOME"));
        for (auto aFile : inputFiles) {
            path aPath(aFile);
            info.emplace_back(std::make_tuple(aPath, rootPath / path("print") /
                                              path(aPath.stem().string() + aPath.extension().string() + ".pdf")));
        }
        return info;
    }

    class CreatePdfFile {
      public:
        explicit CreatePdfFile(const std::string &viewer, const int len)
            : PdfViewer(viewer), Command("sbcode2pdf"), CharactersPerLine(len) {}
        ~CreatePdfFile() {}

        /**
         * Create pdf files of input source files. The output files are
         * generated based on the information from input files.
         *
         */
        void create(const std::vector<std::tuple<path, path>> &info) {
            for (auto val : info) {
                auto inputFile = std::get<0>(val).string();
                auto outputFile = std::get<1>(val).string();
                std::vector<std::string> args = {"-h",
                                                 inputFile,
                                                 "-highlight",
                                                 "-footer",
                                                 inputFile,
                                                 "-chars",
                                                 std::to_string(CharactersPerLine),
                                                 inputFile,
                                                 "-o",
                                                 outputFile};
                utils::run("sbcode2pdf", args);
            }

            // View generated files using given pdf viewer.
            std::vector<std::string> args;
            for (auto val : info) {
                args.emplace_back(std::get<1>(val).string());
            }
            utils::run(PdfViewer, args);
        }

      private:
        std::string PdfViewer;
        const std::string Command;
        int CharactersPerLine;
    };
}

int main(int ac, char *av[]) {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "This command will generate the PDF version of an input file.")(
        "viewer,p", po::value<std::string>(),
        "PDF viewer binary")("input,i", po::value<std::vector<std::string>>(), "The input file")(
        "output,o", po::value<std::string>(), "The output file")("len,n", po::value<int>(),
                                                                 "Number of characters per line");

    po::positional_options_description p;
    p.add("input", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: createPdfFile [options]\n";
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    // Process input parameters
    std::string viewerBinary;
    if (vm.count("viewer")) {
        viewerBinary = vm["viewer"].as<std::string>();
    } else {
        viewerBinary = "/usr/bin/acroread";
    }

    int len;
    if (vm.count("len")) {
        len = vm["len"].as<int>();
    } else {
        len = 120;
    }

    std::vector<std::string> inputFiles;
    if (vm.count("input")) {
        inputFiles = vm["input"].as<std::vector<std::string>>();
    } else {
        std::cout << "Usage: createPdfFile [options]\n";
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    // Generate PDF file.
    CreatePdfFile printer(viewerBinary, len);
    printer.create(generate(inputFiles));
    return EXIT_SUCCESS;
}
