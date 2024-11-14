#pragma once

#include <chrono>
#include <string>
#include <time.h>
#include <filesystem>
#include <sstream>

#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	ExampleAppLog();

	void Clear();

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2);

	void    Draw(bool* p_open = NULL);
};
