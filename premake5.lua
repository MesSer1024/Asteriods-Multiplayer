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
   location "local"

   libdirs { "ExternalLibs/SFML-2.5.1/lib/" }

   filter { "platforms:Static" }
       kind "StaticLib"
       defines { "COMPILING_STATIC" }
	   
	  

   filter { "configurations:Debug" }
      defines { "DEBUG" }
      runtime "Debug"
      staticruntime "on"
      symbols "On"

   filter {"configurations:Release"}
      defines { "RELEASE" }
      runtime "Release"      
      staticruntime "on"
      optimize "On"
      symbols "On"

project "Shared"
   targetdir "bin/%{cfg.buildcfg}"
   includedirs { "source/Shared/", "source/Shared/Public/" }

   files { "source/Shared/**.*"}
   removefiles {}

project "Server"
   kind "ConsoleApp"
   targetdir "bin/%{cfg.buildcfg}"

   links { "Shared" }
   links { 
	"winmm.lib", "wsock32.lib", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-graphics", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-window", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-system", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-network" }
	
	includedirs { "ExternalLibs/SFML-2.5.1/include" }
	
   includedirs {  
		"source/Server/", 
		"source/Server/Public/", 
		"source/Shared/Public/" 
	}

   files { "source/Server/**.*" }

project "Client"
   kind "ConsoleApp"
   targetdir "bin/%{cfg.buildcfg}"

   links { "Shared" }

   links { 
	"winmm.lib", "wsock32.lib", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-graphics", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-window", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-system", 
	"ExternalLibs/SFML-2.5.1/lib/sfml-network" }
	

	includedirs { "ExternalLibs/SFML-2.5.1/include" }
   includedirs { 
	"source/Client/", 
	"source/Client/Public/",
	"source/Shared/Public/"
   }

   files { "source/Client/**.*" }
