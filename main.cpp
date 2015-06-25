#include "gic.h"
#include <QtWidgets/QApplication>
#include <Windows.h>

int clic_main(int argc, char** argv);

int main(int argc, char *argv[])
{
	if (1||GetStdHandle(STD_OUTPUT_HANDLE) == 0) // this is a detached gui process. show gui.
	{
		QApplication a(argc, argv);
		gic w;
		w.show();
		return a.exec();
	}
	else if (argc > 1) // user given command line arguments. treat as cli.
	{
		return clic_main(argc, argv);
	}
	else // create a new detached gui process, and close this one.
	{
		DWORD dwCreationFlags = CREATE_DEFAULT_ERROR_MODE | DETACHED_PROCESS;
		STARTUPINFOA startinfo;
		PROCESS_INFORMATION procinfo;
		ZeroMemory(&startinfo, sizeof(startinfo));
		startinfo.cb = sizeof(startinfo);
		if (!CreateProcessA(NULL, argv[0], NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &startinfo, &procinfo))
			MessageBox(0, TEXT("CreateProcess failed."), TEXT(""), 0);
		exit(0);
	}
}
