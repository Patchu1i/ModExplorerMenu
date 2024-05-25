
#ifndef NANOSVG_IMPLEMENTATION
#	define NANOSVG_IMPLEMENTATION
#	define NANOSVG_ALL_COLOR_KEYWORDS
#	include "nanosvg.h"
#	define NANOSVGRAST_IMPLEMENTATION
#	include "nanosvgrast.h"
#endif

#include "Graphic.h"
#include "Menu.h"

std::map<std::string, GraphicManager::Image> GraphicManager::image_library;
std::map<std::string, ImFont*> GraphicManager::font_library;

bool GraphicManager::GetD3D11Texture(const char* filename, ID3D11ShaderResourceView** out_srv, int& out_width,
	int& out_height)
{
	auto menu = Menu::GetSingleton();
	auto device = menu->GetDevice();

	auto* render_manager = RE::BSGraphics::Renderer::GetSingleton();
	if (!render_manager) {
		logger::error("Cannot find render manager. Initialization failed."sv);
		return false;
	}

	// Load from disk into a raw RGBA buffer
	auto* svg = nsvgParseFromFile(filename, "px", 96.0f);
	auto* rast = nsvgCreateRasterizer();

	auto image_width = static_cast<int>(svg->width);
	auto image_height = static_cast<int>(svg->height);

	auto image_data = (unsigned char*)malloc(image_width * image_height * 4);
	nsvgRasterize(rast, svg, 0, 0, 1, image_data, image_width, image_height, image_width * 4);
	nsvgDelete(svg);
	nsvgDeleteRasterizer(rast);

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* p_texture = nullptr;
	D3D11_SUBRESOURCE_DATA sub_resource;
	sub_resource.pSysMem = image_data;
	sub_resource.SysMemPitch = desc.Width * 4;
	sub_resource.SysMemSlicePitch = 0;

	//auto _device = menu->GetDevice();
	device->CreateTexture2D(&desc, &sub_resource, &p_texture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof srv_desc);
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // old
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;  // old
	srv_desc.Texture2D.MipLevels = desc.MipLevels;           // old
	srv_desc.Texture2D.MostDetailedMip = 0;                  // old


	device->CreateShaderResourceView(p_texture, &srv_desc, out_srv);
	p_texture->Release();

	free(image_data);

	out_width = image_width;
	out_height = image_height;

	return true;
}

void GraphicManager::LoadImagesFromFilepath(std::string a_path, std::map<std::string, Image>& out_struct)
{
	logger::info("[Initialize]: Loading images from directory {}"sv, a_path);

	if (std::filesystem::exists(a_path) == false) {
		auto warning = std::string("FATAL ERROR: Font and/or Graphic asset directory not found. This is because AddItemMenu cannot locate the path '") + a_path + "'. Check your installation.";
		stl::report_and_fail(warning);
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(a_path)) {
		if (entry.path().filename().extension() != ".svg") {
			continue;
		}

		auto index = entry.path().filename().stem().string();  // Get the filename without extension

		GraphicManager::GetD3D11Texture(entry.path().string().c_str(), &out_struct[index.c_str()].texture,
			out_struct[index.c_str()].width, out_struct[index.c_str()].height);
	}

	logger::info("[Initialize]: {} images loaded."sv, out_struct.size());
}

void GraphicManager::LoadFontsFromDirectory(std::string a_path, std::map<std::string, ImFont*>& out_struct)
{
	logger::info("[Initialize]: Loading fonts from directory: {}", a_path);

	if (std::filesystem::exists(a_path) == false) {
		auto warning = std::string("FATAL ERROR: Font and/or Graphic asset directory not found. This is because AddItemMenu cannot locate the path '") + a_path + "'. Check your installation.";
		stl::report_and_fail(warning);
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(a_path)) {
		if (entry.path().filename().extension() != ".tff" && entry.path().filename().extension() != ".otf") {
			continue;  // Pass invalid file types
		}

		auto index = entry.path().filename().stem().string();

		ImGuiIO& io = ImGui::GetIO();
		GraphicManager::font_library[index + "-Small"] = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 16.0f);
		GraphicManager::font_library[index + "-Medium"] = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 20.0f);
		GraphicManager::font_library[index + "-Large"] = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 24.0f);
	}

	logger::info("[Initialize]: {} fonts loaded."sv, GraphicManager::font_library.size());
}

void GraphicManager::Init()
{
	GraphicManager::LoadImagesFromFilepath(std::string("Data/Interface/AddItemMenuNG/images"), GraphicManager::image_library);
	GraphicManager::LoadFontsFromDirectory(std::string("Data/Interface/AddItemMenuNG/fonts"), GraphicManager::font_library);

	GraphicManager::initialized.store(true); // TO-DO Probably not needed.
}