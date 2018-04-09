#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "reader.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;
using namespace reader;

const std::string APPLICATION_NAME = "ThemeParkReader";

void print_usage(string name, const po::options_description &options) {
	cout << "Usage: " << name << " [options] <input file>" << endl;
	cout << "Manipulates Bullfrog Theme Park data files" << endl;
	cout << options << endl;
	cout << "This software comes with absolutely no warranty." << endl;
}

int main(int argc, const char* argv[]) {

	po::options_description usage("Generic");
	usage.add_options()
		("help", "display this message");

	po::options_description actions("Actions");
	actions.add_options()
		("extract-sounds,s", "Extract music or sounds")
		("extract-image,i", "Extract fullscreen image")
		("extract-font", "Extract font")
		("extract-sprites,r", "Extract sprites");

	po::options_description modifiers("Modifiers");
	modifiers.add_options()
		("list,l", "Only list files - do NOT extract")
		("output-dir,o", po::value<std::string>(), "Output directory")
		("palette,p", po::value<std::string>(), "Image palette file");

	po::options_description hidden("Hidden Options");
	hidden.add_options()
		("input-file", po::value<std::string>(), "Input file");

	po::options_description options_desc;
	options_desc.add(usage).add(actions).add(modifiers).add(hidden);

	po::options_description visible;
	visible.add(usage).add(actions).add(modifiers);

	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(options_desc).positional(p).run(), vm);
		po::notify(vm);
	}
	catch (po::error &e) {
		cerr << "Error parsing command-line options: " << e.what() << endl << endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}



	bool validAction = vm.count("extract-sounds") ^ vm.count("extract-image") ^ vm.count("extract-sprites") ^ vm.count("extract-font");

	if (vm.count("help") || vm.size() == 0) {
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	if (!vm.count("list") && !vm.count("output-dir")) {
		cerr << "Output directory not specified" << endl << endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	if (validAction && !vm.count("input-file")) {
		cerr << "Please give an input file" << endl << endl;
		print_usage(APPLICATION_NAME, visible);
		return EXIT_FAILURE;
	}

	std::string inputFile = vm["input-file"].as<std::string>();
	std::string outDir = vm["output-dir"].as<std::string>();


	if (vm.count("extract-image") && vm.count("input-file") && vm.count("palette")) {	
		std::string paletteFile = vm["palette"].as<std::string>();
		return ExtractImage(paletteFile, inputFile, outDir);
	}

	if (inputFile.substr(inputFile.length() - 3, 3).compare("TAB") != 0)
	{
		std::cerr << "Please specify a .TAB input file" << std::endl;
		return EXIT_FAILURE;
	}
	std::string datFile = std::string(inputFile);
	datFile.replace(datFile.length() - 3, 3, "DAT");

	bool listOnly = vm.count("list") > 0;

	if (vm.count("extract-sounds") && vm.count("input-file")) {
		return ExtractMusic(inputFile, datFile, outDir, listOnly);
	}

	if (vm.count("extract-sprites") && vm.count("input-file")) {
		std::string paletteFile = vm["palette"].as<std::string>();
		return ExtractSprites(paletteFile, inputFile, datFile, outDir, listOnly);
	}

	// if (vm.count("extract-font") && vm.count("input-file")) {
	// 	return ExtractFont(inputFile, datFile, outDir);
	// }	

	return EXIT_SUCCESS;

}
