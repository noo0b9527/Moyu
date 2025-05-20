using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using Microsoft.Build.Construction;

class ModuleConfig
{
    public string Name { get; set; }
    public List<string> Sources { get; set; }
    public List<string> Dependencies { get; set; }
    public string Path { get; set; }
    public Guid ProjectGuid { get; set; }
}

class Program
{
    static string SrouceCodeRoot = "src";
    static string SolutionName = "Moyu";

    static void Main()
    {
        Console.WriteLine("[Info] Scanning modules...");
        var modules = LoadModules();

        Console.WriteLine("[Info] Generating projects...");
        foreach (var module in modules.Values)
        {
            GenerateVcxproj(module);
        }

        Console.WriteLine("[Info] Creating solution file...");
        CreateSolution(modules);
    }

    static Dictionary<string, ModuleConfig> LoadModules()
    {
        var result = new Dictionary<string, ModuleConfig>();
        var options = new JsonSerializerOptions
        {
            PropertyNameCaseInsensitive = true
        };

        foreach (var dir in Directory.GetDirectories(SrouceCodeRoot))
        {
            var jsonPath = Path.Combine(dir, "module.json");
            if (!File.Exists(jsonPath)) continue;

            var json = File.ReadAllText(jsonPath);
            var config = JsonSerializer.Deserialize<ModuleConfig>(json, options);
            config.Path = dir;
            config.ProjectGuid = Guid.NewGuid();
            result[config.Name] = config;
        }
        return result;
    }

    static void GenerateVcxproj(ModuleConfig module)
    {
        var project = ProjectRootElement.Create();
        project.ToolsVersion = "";
        project.DefaultTargets = "Build";



        //var props = project.AddPropertyGroup();
        //props.AddProperty("ProjectGuid", module.ProjectGuid.ToString("B").ToUpper());
        //props.AddProperty("ConfigurationType", "Application");
        //props.AddProperty("PlatformToolset", "v143");
        //props.AddProperty("CharacterSet", "Unicode"); 

        var sources = project.AddItemGroup();
        foreach (var src in module.Sources)
        {
            sources.AddItem("ClCompile", src);
        }

        var projRefGroup = project.AddItemGroup();
        foreach (var depName in module.Dependencies)
        {
            if (!string.IsNullOrWhiteSpace(depName))
            {
                var depPath = Path.Combine("..", depName, $"{depName}.vcxproj");
                var refItem = projRefGroup.AddItem("ProjectReference", depPath);
                // GUID will be added later after all modules are loaded
            }
        }

        string projPath = Path.Combine(module.Path, module.Name + ".vcxproj");
        project.Save(projPath);
    }

    static void CreateSolution(Dictionary<string, ModuleConfig> modules)
    {
        var slnPath = Path.Combine(Directory.GetCurrentDirectory(), SolutionName + ".sln");
        if (File.Exists(slnPath)) File.Delete(slnPath);

        System.Diagnostics.Process.Start("dotnet", $"new sln -n {SolutionName}").WaitForExit();

        foreach (var module in modules.Values)
        {
            var projFile = Path.Combine(module.Path, module.Name + ".vcxproj");
            System.Diagnostics.Process.Start("dotnet", $"sln {SolutionName}.sln add \"{projFile}\"").WaitForExit();
        }
    }
}
