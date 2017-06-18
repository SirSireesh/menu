#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

auto delete_type(std::vector<std::string> & args, size_t i, std::string & type_name) {
	while (args[i].find(type_name) < args[i].length())
		args[i] = args[i].substr(0, args[i].find(type_name)) + args[i].substr(args[i].find(type_name) + type_name.length() + 1);

	size_t j = 0;
	while ((j = args[i].find("=")) < args[i].length()) {
		args[i] = args[i].substr(0, j - 1) + args[i].substr(j + 3);
		while (j < args[i].length() && args[i][j] != ' ') {
			args[i] = args[i].substr(0, j) + args[i].substr(j + 1);
		}
	}
}

auto delete_typeinfo(std::vector<std::string> & args, std::string & class_name) {
	for (size_t i = 0; i < args.size(); ++i) {
		std::string type_name = "unsigned double";
		delete_type(args, i, type_name);
		type_name = "unsigned float";
		delete_type(args, i, type_name);
		type_name = "unsigned int";
		delete_type(args, i, type_name);
		type_name = "unsigned char";
		delete_type(args, i, type_name);

		type_name = "unsigned long double";
		delete_type(args, i, type_name);
		type_name = "unsigned long int";
		delete_type(args, i, type_name);

		type_name = "unsigned long long double";
		delete_type(args, i, type_name);
		type_name = "unsigned long long int";
		delete_type(args, i, type_name);

		type_name = "double";
		delete_type(args, i, type_name);
		type_name = "float";
		delete_type(args, i, type_name);
		type_name = "int";
		delete_type(args, i, type_name);
		type_name = "char";
		delete_type(args, i, type_name);
		type_name = "size_t";
		delete_type(args, i, type_name);

		delete_type(args, i, class_name);
	}
}

auto declare(std::vector<std::string> & args, size_t i, std::ofstream & out, std::string type_name) {
	std::string var_name, temp = args[i], args_list;

	if (temp.find(type_name) < temp.length()) {
		out << "\t" << type_name;
		while (temp.find(type_name) < temp.length()) {
			auto j = temp.find(type_name) + type_name.length();
			var_name.clear();
			while (j < temp.length() && temp[j] != ',' /*&& !isspace(temp[j])*/) {
				if (temp[j] == '[') {
					args[i] = args[i].substr(0, args[i].find(temp))
						+ temp.substr(0, temp.find(var_name)) + "*"
						+ temp.substr(temp.find(var_name), temp.find("[") - 4)
						+ temp.substr(temp.find("[") + 2);
					args_list += var_name + " ";
					var_name += "[20]";
					j += 2;
				}
				else
					var_name += temp[j++];
			}
			if (var_name.find("[20]") < var_name.length() && args_list.find(var_name.substr(0, var_name.length() - 4)) < args_list.length()) {
				out << var_name;
				args_list += " " + var_name;
				temp = temp.substr(temp.find(type_name) + type_name.length() + 4);
				if (temp.find(type_name) < temp.length())
					out << ", ";
			} else if (args_list.find(var_name) > args_list.length()) {
				out << var_name;
				args_list += " " + var_name;
				temp = temp.substr(temp.find(type_name) + type_name.length() + 1);
				if (temp.find(type_name) < temp.length())
					out << ", ";
			} else
				temp = temp.substr(temp.find(type_name) + type_name.length() + 1);
		}
		out << ";\n";
	}
}

auto declare_vars(std::vector<std::string> & args, std::ofstream & out, std::string class_name) {
	//TODO: better detect typeinfo than to hardcode it
	std::string temp, args_list;
	for (size_t i = 0; i < args.size(); ++i) {
		if (args[i].length() > 0) {
			temp = args[i];
			declare(args, i, out, "unsigned long long int");

			declare(args, i, out, "unsigned long double");
			declare(args, i, out, "unsigned long int");

			declare(args, i, out, "unsigned double");
			declare(args, i, out, "unsigned float");
			declare(args, i, out, "unsigned int");
			declare(args, i, out, "unsigned char");
			declare(args, i, out, "auto");

			declare(args, i, out, "double");
			declare(args, i, out, "float");
			declare(args, i, out, "int");
			declare(args, i, out, "char");
			declare(args, i, out, "size_t");
			declare(args, i, out, class_name);
		}
	}
}

auto print_main(std::ofstream & out, std::vector<std::string> & args, std::vector<std::string> & func_names, std::string & menu,
		std::string & class_name, std::string & obj_name, int opt_number) {
	std::string temp;

	out << "\nint main()\n{\n\t" << class_name << ' ' << obj_name << ";\n";

	declare_vars(args, out, class_name);

	out << "\n\tint op;\n\tdo {\n";
	out << "\t\tsystem(\"cls\");\n\t\tstd::cout << \"" << menu << opt_number << ". Exit\\n\";\n";
	out << "\t\tstd::cout << \"$ \";\n";
	out << "\t\tstd::cin >> op;\n\t\tstd::cin.ignore(1000, '\\n');\n\t\tswitch (op) {\n";

	delete_typeinfo(args, class_name);

	for (int i = 0; i < opt_number - 1; ++i) {
		out << "\t\t\tcase " << i + 1 << " :\n";
		temp.clear();

		auto temp = args[i];
		while (temp.length() > 0) {
			auto loc = temp.find(",");

			if (temp[0] != '*') {
				out << "\t\t\t\tstd::cout << \"Enter the value for ";
				out << temp.substr(0, loc > temp.length() ? temp.length() : loc) << " :\";\n";
				out << "\t\t\t\tstd::cin >> ";

				if (temp.find(",") < temp.length())
					out << temp.substr(0, temp.find(","));
				else
					out << temp;

				out << ";\n";
				out << "\t\t\t\tstd::cin.ignore(1000, '\\n');\n\n";
			} else {
				args[i] = args[i].substr(0, args[i].find(temp)) + temp.substr(1);
			}

			if (temp.find(" ") < temp.length())
				temp = temp.substr(temp.find(" ") + 1);
			else
				break;
		}

		out << "\t\t\t\t" << obj_name << "." << func_names[i] << args[i] << ");";
		out << "\n\t\t\t\tstd::cout << \"Press Enter to continue ... \";";
		out << "\n\t\t\t\tstd::cin.get();";
		out << "\n\t\t\t\tstd::cin.ignore(1000, '\\n');";
		out << "\n\t\t\t\tbreak;\n";
	}
	out << "\t\t\tcase " << opt_number << " :" << "\n\t\t\t\treturn 0;\n";
	out << "\t\t\tdefault :\n\t\t\t\tstd::cout << \"Invalid option!\";\n\t\t\t\tstd::cin.get();\n\t\t\t\tbreak;\n";
	out << "\t\t}\n\t} while(true);\n}\n";
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cout << argv[0] << "ill formed call\nusage : menu [file]\n";
		return 1;
	}

	std::string filename = argv[1];
	std::ifstream input(filename);
	std::ofstream out(filename.substr(0, filename.length() - 4) + " menu.cpp");
	std::string menu, temp, class_name, obj_name = "obj", line;
	std::vector<std::string> func_names(0), args(0);
	int opt_number = 1;
	filename = filename.substr(0, filename.length() - 4);

	out << "#include <cstdlib>\n";
	while (std::getline(input, line)) {
		if (line.length() > 8 && line.find("class") < line.length() - 6) {
			for (size_t i = 6; i < line.length() && line[i] != ' '; ++i)
				class_name += line[i];
			if (line.find("//") < line.length() - 2)
				obj_name = line.substr(line.find("//") + 2);
		} else if (line.length() > 2 && line.find(std::to_string(opt_number) + ".") < line.length()) {
			menu += line.substr(line.find("//" + std::to_string(opt_number++)) + 2);
			menu += "\\n";

			temp.clear();
			size_t i = 0;
			while (line[i++] != '(')
				;
			--i;
			while (line[i] != ' ')
				temp += line[i--];
			std::reverse(temp.begin(), temp.end());
			func_names.push_back(temp);

			temp.clear();
			i = 0;
			while (line[i++] != '(')
				;
			while (line[i] != ')' && i < line.length()) {
				temp += line[i++];
			}
			args.push_back(temp);
		}
		out << line << '\n';
	}
	if (class_name.empty() || func_names.empty() || args.empty() || func_names.empty() || opt_number == 0) {
		std::cerr << "Insuficient info in file!!!\n";
		std::cerr << "Info Dump :\n";
		std::cerr << "Class Name : " << class_name << std::endl;
		std::cerr << "Function list :\n";
		for (auto name : func_names)
			std::cerr << name << std::endl;
		std::cerr << "Function args list :";
		for (auto arg : args)
			std::cerr << arg << std::endl;
		std::cerr << std::endl;
		std::cerr << "Menu options count : " << opt_number << std::endl;
		std::cerr << "Object name : " << obj_name << std::endl;
		input.close();
		out.close();
		return 1;
	}

	print_main(out, args, func_names, menu, class_name, obj_name, opt_number);
	input.close();
	out.close();
}
