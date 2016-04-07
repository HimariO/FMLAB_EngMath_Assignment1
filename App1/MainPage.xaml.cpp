//
// MainPage.xaml.cpp
// MainPage 類別的實作。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <codecvt>
#include <regex>
#include <algorithm>
#include <sstream>


using namespace App1;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
//using namespace System::Text::RegularExpressions;

//空白頁項目範本收錄在 http://go.microsoft.com/fwlink/?LinkId=402352clcid=0x409




MainPage::MainPage()
{
	InitializeComponent();
	

	A = ref new Platform::Collections::Vector<VarDisplay^>();
	TestAuto = ref new Platform::Collections::Vector<String^>{
		"vectest",
		"mtxtest",
		"dot",
		"rref",
		"trans",
		"slice",
		"det",
		"cofactor",
		"adj",
		"inverse",
		"up_tri",
		"low_tri",
		"solveLinearSys",
		"LeastSquare",
		"pickfolder",
		"loadfile",
	};
	OutputList = ref new Platform::Collections::Vector<OutputDisplay^>();
	parser = new Parser();

	listView1->ItemsSource = A;
	CenterllistView->ItemsSource = OutputList;

	auto currdir = Windows::Storage::ApplicationData::Current;
	auto coreTitleBar =  Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->TitleBar;
	coreTitleBar->ExtendViewIntoTitleBar = true;
	
}


std::string  STos(String^ str) {
	std::wstring ws = str->Data();
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(ws);
}

String^  sToS(std::string str) {
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	std::wstring ws = converterX.from_bytes(str);

	return ref new String(ws.c_str());
}


void App1::MainPage::PickFile() {
	using namespace Windows::Storage;

	auto picker =ref new Pickers::FolderPicker();
	picker->ViewMode =Pickers::PickerViewMode::Thumbnail;
	picker->SuggestedStartLocation = Pickers::PickerLocationId::Desktop;
	picker->FileTypeFilter->Append(L".txt");

	auto init_task = concurrency::create_task(picker->PickSingleFolderAsync());

	init_task.then([this](StorageFolder^ folder)  {
		this->DirList->Header = folder->DisplayName;
		this->working_dir = folder;

		concurrency::create_task(folder->GetFilesAsync()).then([this](IVectorView<StorageFile^>^ fileView) {
			this->DirList->Items->Clear();
			for (auto file : fileView) {
				this->DirList->Items->Append(ref new OutputDisplay("", file->DisplayName+file->FileType));
			}
		});
	});
};


void App1::MainPage::UpdateVarList() {

	std::string var_type = "";
	std::vector<Variable*> new_find;

	if (parser->scope.size() > nLocalVar) {

		for (auto pair : parser->scope) {
			if (std::find(var_list.begin(), var_list.end(), pair.first) == var_list.end()) {
				new_find.push_back( pair.second);
				
			}
		}
		nLocalVar = parser->scope.size();

	}
	else if (parser->global_scope.size() > nGlobalVar) {
		for (auto pair : parser->global_scope) {
			if (std::find(var_list.begin(), var_list.end(), pair.first) == var_list.end()) {
				new_find.push_back(pair.second);
				
			}
		}
		nGlobalVar = parser->global_scope.size();
	}



	for (auto new_come : new_find) {
		if (new_come->gettype() == vType::number) {
			var_type = "Number";
		}
		else if (new_come->gettype() == vType::vector) {
			vVector *temp = dynamic_cast<vVector*>(new_come);
			if (temp->isMarix())
				var_type = "Matrix";
			else
				var_type = "Vector";
		}

		var_list.push_back(new_come->name);
		A->Append(%VarDisplay(sToS(new_come->name), sToS(var_type)));
		TestAuto->Append(sToS(new_come->name));
	}
};


void App1::MainPage::LoadDataFile(std::string path) {
	using namespace concurrency;

	create_task(this->working_dir->GetFileAsync(sToS(path))).then([this](StorageFile^ file) {
		try {
			return FileIO::ReadTextAsync(file);
		}
		catch (Platform::COMException^ e) {
			this->OutputList->Append(ref new OutputDisplay("", "File not Found."));
		}
	}).then([this, path](task<String^> task) {
		try {
			String ^text = task.get();
			std::string file_name = std::regex_replace(path, std::regex("\\.\\w+$"), "");
			this->OutputList->Append(ref new OutputDisplay("", "File Loading."));

			std::string sd_text = STos(text);
			std::stringstream ss(sd_text);
			std::string line;

			std::getline(ss, line);
			int variable_num = std::stoi(line);

			while (std::getline(ss, line)) {
				if (line == "M" || line == "m") {
					std::getline(ss, line);

					std::string m_size = line;
					std::smatch match;
					std::regex_search(m_size, match, std::regex("(\\d+)\\s+(\\d+)"));

					std::string debug1 = (match[1]), debug2 = (match[2]);
					int N = std::stoi(match[1]), M = std::stoi(match[2]);

					std::vector < std::vector<double>> matrix;
					for (int n = 0; n < N; n++) {
						std::vector<double> row;
						std::getline(ss, line);
					
						for (int m = 0; m < M; m++) {
							std::regex_search(line, match, std::regex("(\\-?\\d+\\.?\\d*)"));
							row.push_back(std::stod(match[1]));
							line.replace(0, match.position() + match[1].length(), "");
						}
						matrix.push_back(row);
					}

					vVector *mtx = new vVector();
					mtx->setMatrixData(matrix);
					mtx->name = file_name + "_" + std::to_string(this->parser->scope.size());
					this->OutputList->Append(ref new OutputDisplay("", "Load   " + sToS(mtx->name)));
					this->parser->udpate_variable(mtx);
				}
				else if(line =="V" || line=="v"){
					std::getline(ss, line);

					std::string m_size = line;
					std::smatch match;
					std::regex_search(m_size, match, std::regex("(\\d+)"));
					int L = std::stoi(match[1]);

					std::vector<double> row;
					std::getline(ss, line);
					
					for (int m = 0; m < L; m++) {
						std::regex_search(line, match, std::regex("(\\-?\\d+\\.?\\d*)"));
						row.push_back(std::stod(match[1]));
						line.replace(0, match.position() + match[1].length(), "");
					}

					vVector *mtx = new vVector();
					mtx->setVectorData(row);
					mtx->name = file_name + "_" + std::to_string(this->parser->scope.size());
					this->parser->udpate_variable(mtx);
				}
			}

			this->OutputList->Append(ref new OutputDisplay("", variable_num + " Variable Loaded."));
			UpdateVarList();
		}
		catch (Platform::COMException^ e) {
			//do nothing
		}
	});
};


void App1::MainPage::SplitView_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	auto daa = e->Data;

}


void App1::MainPage::SplitView_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args)
{
	//App1::MainPage::dir_view->SetValue()
	/*DependencyProperty ^dpv = App1::MainPage::dir_view->MarginProperty;
	dir_view->SetValue(dpv, )*/
	//dir_view->Margin = new Thickness(0, 0, 0, 0);
}


void App1::MainPage::dir_view_DropCompleted(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args)
{
	
	auto restul = args->DropResult;

}


void App1::MainPage::Grid_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
{
	auto daa = e->Data;

}



void App1::MainPage::AutoSuggestBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	
}


void App1::MainPage::AutoSuggestBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	auto key = e->Key;

	if (key == Windows::System::VirtualKey::Enter && CommandLine->Text != "") {

		OutputDisplay ^out;

		try {
			std::string input_str = STos(CommandLine->Text);

			if (input_str == "pickfolder") {
				PickFile();
			}else if (input_str == "cl") {
				OutputList->Clear();
			}
			else if (std::regex_match(input_str.begin(), input_str.end(), std::regex("^loadfile\\s+[\\w\\.]+"))) {
				std::smatch file_m;
				std::regex_search(input_str, file_m, std::regex("^loadfile\\s+([\\w\\.]+)"));
				LoadDataFile(file_m[1]);
			}
			else {
				Variable *result = parser->parseRaw(STos(CommandLine->Text));
				std::string cache;
				if (result->gettype() == vType::vector) {
					vVector *a = dynamic_cast<vVector*>(result);
				}
				cache = result->toString();
				out = ref new OutputDisplay(CommandLine->Text, sToS(result->toString()));

				UpdateVarList();
				OutputList->Append(out);
			}
		}
		catch (std::string error) {
			out = ref new OutputDisplay(CommandLine->Text, sToS(error));
			OutputList->Append(out);
		}

		CommandLine->Text = "";
	}
}


void App1::MainPage::CommandLine_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args)
{
	if (args->Reason == Windows::UI::Xaml::Controls::AutoSuggestionBoxTextChangeReason::UserInput 
		&& CommandLine->Text!="") {
		Windows::Foundation::Collections::IVector<String^> ^filtered = ref new Platform::Collections::Vector<String^>();
		
		std::string input = STos(CommandLine->Text);
		std::string input_filtered;

		std::smatch wordonly_m;
		std::regex_search(input, wordonly_m, std::regex("\\w+"));

		if (wordonly_m.length() > 0) {
			input_filtered = wordonly_m[0];

			for (auto vname : TestAuto) {
				std::string std_name = STos(vname);
				if (std::regex_match(std_name.begin(), std_name.end(), std::regex(".*" + input_filtered+".*")))
					filtered->Append(vname);
			}
		}
		sender->ItemsSource = filtered;
	}
}


void App1::MainPage::CommandLine_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args)
{

}


void App1::MainPage::CommandLine_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
{

}

