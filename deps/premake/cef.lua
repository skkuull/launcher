cef = {
	source = "deps/cef"
}

function cef.import()
	filter {"kind:not StaticLib" }
	links { "cef", "cef_sandbox", "libcef" }
	linkoptions { "/DELAYLOAD:libcef.dll" }
	filter {}
	cef.includes()
end

function cef.includes()
	includedirs { cef.source }
	defines {
		"WRAPPING_CEF_SHARED",
		"NOMINMAX",
		"USING_CEF_SHARED",
	}
	
	filter { "Release" }
		libdirs { path.join(cef.source, "Release") }
	filter { "Debug" }
		libdirs { path.join(cef.source, "Debug") }
	filter {}
end

function cef.project()
	project "cef"
		language "C++"

		cef.includes()
		files
		{
			path.join(cef.source, "libcef_dll/**.h"),
			path.join(cef.source, "libcef_dll/**.cc"),
		}

		postbuildcommands {
			"mkdir \"%{wks.location}runtime/%{cfg.platform}/%{cfg.buildcfg}/cef/\" 2> nul",
			"mkdir \"%{wks.location}runtime/%{cfg.platform}/%{cfg.buildcfg}/cef/locales/\" 2> nul",
			"copy /y \"%{wks.location}..\\deps\\cef\\%{cfg.buildcfg}\\*.dll\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\%{cfg.buildcfg}\\*.bin\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\*.pak\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\*.dat\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\\cef\\\"",
			"copy /y \"%{wks.location}..\\deps\\cef\\Resources\\locales\\*.pak\" \"%{wks.location}runtime\\%{cfg.platform}\\%{cfg.buildcfg}\\cef\\locales\\\"",
		}

		linkoptions { "-IGNORE:4221", "-IGNORE:4006" }
		removelinks "*"
		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, cef)
