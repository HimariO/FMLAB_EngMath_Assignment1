//
// MainPage.xaml.h
// MainPage 類別的宣告。
//

#pragma once

#include "MainPage.g.h"
#include "UIdataContainer.h"
#include "Parser.h"

using namespace Platform;
using namespace Windows::Storage;

namespace App1
{

	/// <summary>
	/// 可以在本身使用或巡覽至框架內的空白頁面。
	/// </summary>
	public ref class MainPage sealed
	{	
	public:
		MainPage();
		property Windows::Foundation::Collections::IVector<VarDisplay^> ^A ;
		property Windows::Foundation::Collections::IVector<OutputDisplay^> ^OutputList;
		
		
	private:
		property Windows::Foundation::Collections::IVector<String^> ^TestAuto;
		Parser *parser;
		std::vector<std::string> var_list;
		StorageFolder^ working_dir;

		int nLocalVar = 0;
		int nGlobalVar = 0;

		void PickFile();
		void LoadDataFile(std::string path);
		void UpdateVarList();
		void SplitView_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void SplitView_DragStarting(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DragStartingEventArgs^ args);
		void dir_view_DropCompleted(Windows::UI::Xaml::UIElement^ sender, Windows::UI::Xaml::DropCompletedEventArgs^ args);
		void Grid_DragEnter(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void AutoSuggestBox_KeyDown(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void AutoSuggestBox_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void CommandLine_TextChanged(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs^ args);
		void CommandLine_SuggestionChosen(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs^ args);
		void CommandLine_QuerySubmitted(Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args);
		void DirList_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

	};
}
