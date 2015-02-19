#include <string>
#include <iostream>
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "utils/Resources.hpp"
#include "utils/Basic.hpp"
#include "utils/FileSystemUtilities.hpp"

class CreatePdfFile
{
  public:
    explicit CreatePdfFile(const std::string & viewer, const std::string & outputFile, const int len) : 
        PdfViewer(viewer), OutputFile(outputFile), Command("sbcode2pdf"), CharactersPerLine(len) {}
    ~CreatePdfFile() {}
    
    void create(const std::string & inputFile)
    {
        std::string command;
        
        // Generate an output PDF file
        command = Command + 
            " -h \"\" -highlight -footer \"\" -chars " + boost::lexical_cast<std::string>(CharactersPerLine) + 
            " " + inputFile + " -o " + OutputFile;
        std::cout << command << std::endl;
        Tools::run(command);
        
        // View a generated PDF file
        command = PdfViewer + " " + OutputFile + "&";
        std::cout << command << std::endl;
        Tools::run(command);
    }
    
  private:
    std::string PdfViewer;
    std::string OutputFile;
    std::string Command;
    int CharactersPerLine;
};


int main(int ac, char* av[])
{
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "This command will generate the PDF version of an input file.")
        ("viewer,p", po::value<std::string>(), "PDF viewer binary")
        ("input,i", po::value<std::string>(), "The input file")
        ("output,o", po::value<std::string>(), "The output file")
        ("len,n", po::value<int>(), "Number of characters per line")
        ;

    po::positional_options_description p;
    p.add("input", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << "Usage: createPdfFile [options]\n";
        std::cout << desc;
        return false;
    }

    std::string inputFile;
    if (vm.count("input"))
    {
        inputFile = vm["input"].as<std::string>();
    }
    else
    {
        std::cout << "Usage: createPdfFile [options]\n";
        std::cout << desc;
        return false;
    }

    std::string outputFile;
    if (vm.count("output"))
    {
        outputFile = vm["output"].as<std::string>();
    }
    else
    {
        // Generate the default output file using the printed file name
        const boost::filesystem::path printedFile(inputFile);
        const std::string suggestedName = printedFile.filename().string();
        outputFile = Tools::Resources<std::string>::PrintFolder + Tools::Resources<std::string>::FileSeparator + suggestedName + ".pdf";
    }

    std::string viewerBinary;
    if (vm.count("viewer"))
    {
        viewerBinary = vm["viewer"].as<std::string>();
    }
    else
    {
        viewerBinary = "/usr/bin/acroread";
    }

    int len;
    if (vm.count("len"))
    {
        len = vm["len"].as<int>();
    }
    else
    {
        len = 120;
    }

    // Generate PDF file.
    CreatePdfFile printer(viewerBinary, outputFile, len);
    printer.create(inputFile);

    return EXIT_SUCCESS;
}
