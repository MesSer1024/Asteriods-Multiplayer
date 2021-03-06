-- AsteroidsMultiplayer/premake5.lua

function os.winSdkVersion()
    local reg_arch = iif( os.is64bit(), "\\Wow6432Node\\", "\\" )
    local sdk_version = os.getWindowsRegistry( "HKLM:SOFTWARE" .. reg_arch .."Microsoft\\Microsoft SDKs\\Windows\\v10.0\\ProductVersion" )
    if sdk_version ~= nil then return sdk_version end
end

-- <Workspace Settings>

workspace "AsteroidsMP"
	filename "AsteroidsMP"
	language "C++"
	configurations { "Debug", "Release" }
	platforms { "Static" }
	architecture "x64"   
	cppdialect "C++17"
	
	location "local" -- project folder [sln-files ...]
	targetdir "bin/%{cfg.buildcfg}" -- output directory

	libdirs { "ExternalLibs/SFML-2.5.1/lib/" }
	defines { "NOMINMAX" } -- avoid issues with windows defining min/max macro
	
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
	  
	filter {"system:windows", "action:vs*"}
		systemversion(os.winSdkVersion() .. ".0")
-- </Workspace Settings>

-- <UtilityFunctions>
function DeclareProject(identifier, projectType)
	project (identifier)
	if projectType ~= nil then kind (projectType) else kind ("StaticLib") end
	
	files { "source/" .. identifier .. "/**" }
	includedirs { "source/" .. identifier, "source/" .. identifier .. "/Public"  }
end

function DeclareTestProject(identifier)
	project (identifier)
	kind "ConsoleApp"
	
	files { "source/" .. identifier .. "/**" }
	includedirs { "source/" .. identifier, "ExternalLibs/googletest/include" }		
	links { "GoogleTest" }
end

function AddOneDependency(name)
	links { name }
	includedirs { "source/" .. name .. "/Public" }
end

function AddDependency(...)
   local arg = {...}
   for i,v in ipairs(arg) do
      AddOneDependency(v)
   end
end

function AddLinkToSFML()
	   links { 
		"winmm.lib", "wsock32.lib", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-graphics", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-window", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-system", 
		"ExternalLibs/SFML-2.5.1/lib/sfml-network" }
		
		includedirs { "ExternalLibs/SFML-2.5.1/include" }
end		
-- </UtilityFunctions>

-- <ExternalProjects>
group "_External"

  project "GoogleTest"
    kind "StaticLib"
    files { "ExternalLibs/googletest/src/gtest-all.cc" }
    includedirs { "ExternalLibs/googletest/include", "ExternalLibs/googletest" }

group ""
-- </ExternalProjects>
	  
-- <LibraryProjects>
group "Library"
	DeclareProject("Network", "StaticLib")
		defines { "BUILD_EXPORT_NETWORK_MODULE"}

	group "Library/Tests"	 
		DeclareTestProject("Network.Test")
		AddDependency("Network")
		defines { "BUILD_INTERNAL_ACCESS_NETWORK_MODULE"}
group "" -- leave Library-group
-- </LibraryProjects>

-- <AsteroidsProjects>
group "Asteroids"
	DeclareProject("Shared", "StaticLib")
		AddDependency("Network")

	DeclareProject("Server", "ConsoleApp")
		targetname "AsteroidsServer"
		AddDependency("Shared", "Network")
		AddLinkToSFML()
		filter { "configurations:Debug" }
			defines { "FAKE_LAG" } 
		filter {}

	DeclareProject("Client", "ConsoleApp")
	   targetname "AsteroidsClient"
	   AddDependency("Shared", "Network")
	   AddLinkToSFML()
group "" 
-- </AsteroidsProjects>
