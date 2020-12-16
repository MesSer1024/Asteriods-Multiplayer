-- premake5.lua

function os.winSdkVersion()
    local reg_arch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
    local sdk_version = os.getWindowsRegistry( "HKLM:SOFTWARE" .. reg_arch .."Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )
    if sdk_version ~= nil then return sdk_version end
end

-- ///////////////////////////////////////

workspace "AsteroidsMP"
   filter {"system:windows", "action:vs*"}
      systemversion(os.winSdkVersion() .. ".0")

   filename "AsteroidsMP"
   language "C++"
   configurations { "Debug", "Release" }
   platforms { "Static" }
   architecture "x64"   
	cppdialect "C++17"
   location "local"

   libdirs { "ExternalLibs/SFML-2.5.1/lib/" }

   filter { "platforms:Static" }
       kind "StaticLib"
       defines { "COMPILING_STATIC" }
	   
	  

   filter { "configurations:Debug" }
      defines { "DEBUG", "PACKAGE_LOSS" }
      runtime "Debug"
      staticruntime "on"
      symbols "On"

   filter {"configurations:Release"}
      defines { "RELEASE" }
      runtime "Release"      
      staticruntime "on"
      optimize "On"
      symbols "On"

-- Done with global project settings
group "_External"

  project "GoogleTest"
    kind "StaticLib"
    files { "ExternalLibs/googletest/src/gtest-all.cc" }
    includedirs { "ExternalLibs/googletest/include", "ExternalLibs/googletest" }

group ""  -- leave External-group
	  

group "Library"
	project "Network"
	kind "StaticLib"
	files { "source/Network/**" }

	includedirs { "source/Network/Public", "source/Network" }
	defines { "BUILD_EXPORT_NETWORK_MODULE"}

group "Library/Tests"
  project "Network.Test"
    kind "ConsoleApp"
    defines { "BUILD_INTERNAL_ACCESS_NETWORK_MODULE"}
	targetdir "bin/%{cfg.buildcfg}"
	
    files { "source/Network.Test/**" }

    links { "Network", "GoogleTest" }
	includedirs { 
		"source/Network.Test", 
		"source/Network/Public", 
		"source/Network", 
		"ExternalLibs/googletest/include" 
	}
group "" -- leave Library-group

group "Asteroids"
	project "Shared"
	   targetdir "bin/%{cfg.buildcfg}"
	   includedirs { "source/Network/Public", "source/Shared/", "source/Shared/Public/" }

	   files { "source/Shared/**.*"}
	   
	   links { "Network" }

	project "Server"
	   kind "ConsoleApp"
	   targetdir "bin/%{cfg.buildcfg}"
	   targetname "Asteroids Server"
	   
	   filter { "configurations:Debug" }
		defines { "FAKE_LAG" } 
		filter {}

	   links { "Shared", "Network" }
	   links { 
		"winmm.lib", "wsock32.lib", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-graphics", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-window", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-system", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-network" }
		
		includedirs { "ExternalLibs/SFML-2.5.1/include" }
		
	   includedirs {  
			"source/Network/Public",
			"source/Server/", 
			"source/Server/Public/", 
			"source/Shared/Public/" 
		}

	   files { "source/Server/**.*" }

	project "Client"
	   kind "ConsoleApp"
	   targetdir "bin/%{cfg.buildcfg}"
	   targetname "Asteroids Client"

	   links { "Shared", "Network" }

	   links { 
		"winmm.lib", "wsock32.lib", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-graphics", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-window", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-system", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-network" }
		

		includedirs { "ExternalLibs/SFML-2.5.1/include" }
	   includedirs { 
	    "source/Network/Public",
		"source/Client/", 
		"source/Client/Public/",
		"source/Shared/Public/"
	   }

	   files { "source/Client/**.*" }
group "" -- leave Asteroids-scope
