#include "pch.h"
#include "Texture.h"

#include <SDL_opengl.h>
#include <iostream>
#include <string>

Texture::Texture(const std::string& imagePath)
	:m_Id{ }
	, m_Width{ 10.0f }
	, m_Height{ 10.0f }
	, m_CreationOk{ false }
{
	CreateFromImage(imagePath);
}

Texture::Texture(const std::string& text, TTF_Font* pFont, const Color4f& textColor)
	:m_Id{}
	, m_Width{ 10.0f }
	, m_Height{ 10.0f }
	, m_CreationOk{ false }
{
	CreateFromString(text, pFont, textColor);
}

Texture::Texture(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
	:m_Id{}
	, m_Width{ 10.0f }
	, m_Height{ 10.0f }
	, m_CreationOk{ false }
{
	CreateFromString(text, fontPath, ptSize, textColor);
}
Texture::Texture(Texture&& other) noexcept
	:m_Id{ other.m_Id }
	, m_Width{ other.m_Width }
	, m_Height{ other.m_Height }
	, m_CreationOk{ other.m_CreationOk }
{
	other.m_Id = 0;
	other.m_CreationOk = false;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other)// no self assignment
	{
		m_Id = other.m_Id;
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		m_CreationOk = other.m_CreationOk;
		other.m_Id = 0;
		other.m_CreationOk = false;
	}
	return *this;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_Id);
}

void Texture::CreateFromImage(const std::string& path)
{
	m_CreationOk = true;

	// Load image at specified path
	SDL_Surface* pLoadedSurface = IMG_Load(path.c_str());
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "Texture::CreateFromImage, error when calling IMG_Load: " << SDL_GetError() << std::endl;
		m_CreationOk = false;
		return;
	}
	CreateFromSurface(pLoadedSurface);

	// Free loaded surface
	SDL_FreeSurface(pLoadedSurface);
}

void Texture::CreateFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor)
{
	m_CreationOk = true;

	// Create font
	TTF_Font* pFont{};
	pFont = TTF_OpenFont(fontPath.c_str(), ptSize);
	if (pFont == nullptr)
	{
		std::cerr << "Texture::CreateFromString, error when calling TTF_OpenFont: " << TTF_GetError() << std::endl;
		m_CreationOk = false;
		return;
	}

	// Create texture using this font and close font afterwards
	CreateFromString(text, pFont, textColor);
	TTF_CloseFont(pFont);
}

void Texture::CreateFromString(const std::string& text, TTF_Font* pFont, const Color4f& color)
{
	m_CreationOk = true;
	if (pFont == nullptr)
	{
		std::cerr << "Texture::CreateFromString, invalid TTF_Font pointer\n";
		m_CreationOk = false;
		return;
	}

	// Render text surface
	SDL_Color textColor{};
	textColor.r = static_cast<Uint8>(color.r * 255);
	textColor.g = static_cast<Uint8>(color.g * 255);
	textColor.b = static_cast<Uint8>(color.b * 255);
	textColor.a = static_cast<Uint8>(color.a * 255);

	SDL_Surface* pLoadedSurface = TTF_RenderText_Blended(pFont, text.c_str(), textColor);
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "Texture::CreateFromString, error when calling TTF_RenderText_Blended: " << TTF_GetError() << std::endl;
		m_CreationOk = false;
		return;
	}

	// Copy to video memory
	CreateFromSurface(pLoadedSurface);

	// Free loaded surface
	SDL_FreeSurface(pLoadedSurface);
}

void Texture::CreateFromSurface(SDL_Surface* pSurface)
{
	m_CreationOk = true;

	//Get image dimensions
	m_Width = static_cast<float>(pSurface->w);
	m_Height = static_cast<float>(pSurface->h);

	// Get pixel format information and translate to OpenGl format
	GLenum pixelFormat{ GL_RGB };
	switch (pSurface->format->BytesPerPixel)
	{
	case 3:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGB;
		}
		else
		{
			pixelFormat = GL_BGR;
		}
		break;
	case 4:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGBA;
		}
		else
		{
			pixelFormat = GL_BGRA;
		}
		break;
	default:
		std::cerr << "Texture::CreateFromSurface, unknow pixel format, BytesPerPixel: " << pSurface->format->BytesPerPixel << "\nUse 32 bit or 24 bit images.\n";
		m_CreationOk = false;
		return;
	}

	//Generate an array of textures.  We only want one texture (one element array), so trick
	//it by treating "texture" as array of length one.
	glGenTextures(1, &m_Id);

	//Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
	//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
	glBindTexture(GL_TEXTURE_2D, m_Id);
	// check for errors. Can happen if a texture is created while a static pointer is being initialized, even before the call to the main function.
	const GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "Texture::CreateFromSurface, error binding textures, Error id = " << e << '\n';
		std::cerr << "Can happen if a texture is created before performing the initialization code (e.g. a static Texture object).\n";
		std::cerr << "There might be a white rectangle instead of the image.\n";
	}

	// Specify the texture's data.
	// This function is a bit tricky, and it's hard to find helpful documentation.
	// A summary:
	//    GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
	//                0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
	//                         not cached smaller copies)
	//          GL_RGBA:    Specifies the number of color components in the texture.
	//                     This is how OpenGL will store the texture internally (kinda)--
	//                     It's essentially the texture's type.
	//       surface->w:    The width of the texture
	//       surface->h:    The height of the texture
	//                0:    The border.  Don't worry about this if you're just starting.
	//      pixelFormat:    The format that the *data* is in--NOT the texture!
	// GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
	//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
	//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
	//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
	//  surface->pixels:    The actual data.  As above, SDL's array of bytes.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

	// Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
	// *smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
	// each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
	// further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
	// them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::Draw(const Point2f& dstBottomLeft, const Rectf& srcRect) const
{
	const float epsilon{ 0.001f };
	if (!m_CreationOk)
	{
		if (!(srcRect.width > epsilon && srcRect.height > epsilon)) // No srcRect specified
		{
			DrawFilledRect(Rectf{ dstBottomLeft.x, dstBottomLeft.y, m_Width, m_Height });
		}
		else
		{
			DrawFilledRect(Rectf{ dstBottomLeft.x, dstBottomLeft.y, srcRect.width, srcRect.height });
		}
	}
	else
	{
		Rectf dstRect{ dstBottomLeft.x, dstBottomLeft.y, srcRect.width, srcRect.height };
		if (!(srcRect.width > epsilon && srcRect.height > epsilon)) // No srcRect specified
		{
			dstRect.width = m_Width;
			dstRect.height = m_Height;
		}
		Draw(dstRect, srcRect);
	}
}

void Texture::Draw(const Rectf& dstRect, const Rectf& srcRect) const
{
	const float epsilon{ 0.001f };
	if (!m_CreationOk)
	{
		DrawFilledRect(dstRect);
		return;
	}

	// Determine texture coordinates using srcRect and default destination width and height
	float textLeft{};
	float textRight{};
	float textTop{};
	float textBottom{};

	float defaultDestWidth{};
	float defaultDestHeight{};
	if (!(srcRect.width > epsilon && srcRect.height > epsilon)) // No srcRect specified
	{
		// Use complete texture
		textLeft = 0.0f;
		textRight = 1.0f;
		textTop = 0.0f;
		textBottom = 1.0f;

		defaultDestHeight = m_Height;
		defaultDestWidth = m_Width;
	}
	else // srcRect specified
	{
		// Convert to the range [0.0, 1.0]
		textLeft = srcRect.left / m_Width;
		textRight = (srcRect.left + srcRect.width) / m_Width;
		textTop = (srcRect.bottom - srcRect.height) / m_Height;
		textBottom = srcRect.bottom / m_Height;

		defaultDestHeight = srcRect.height;
		defaultDestWidth = srcRect.width;
	}

	// Determine vertex coordinates
	const float vertexLeft{ dstRect.left };
	const float vertexBottom{ dstRect.bottom };
	float vertexRight{};
	float vertexTop{};
	if (!(dstRect.width > 0.001f && dstRect.height > 0.001f)) // If no size specified use default size
	{
		vertexRight = vertexLeft + defaultDestWidth;
		vertexTop = vertexBottom + defaultDestHeight;
	}
	else
	{
		vertexRight = vertexLeft + dstRect.width;
		vertexTop = vertexBottom + dstRect.height;
	}

	// Tell opengl which texture we will use
	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Draw
	glEnable(GL_TEXTURE_2D);
	{
		glBegin(GL_QUADS);
		{
			glTexCoord2f(textLeft, textBottom);
			glVertex2f(vertexLeft, vertexBottom);

			glTexCoord2f(textLeft, textTop);
			glVertex2f(vertexLeft, vertexTop);

			glTexCoord2f(textRight, textTop);
			glVertex2f(vertexRight, vertexTop);

			glTexCoord2f(textRight, textBottom);
			glVertex2f(vertexRight, vertexBottom);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

float Texture::GetWidth() const
{
	return m_Width;
}

float Texture::GetHeight() const
{
	return m_Height;
}

bool Texture::IsCreationOk() const
{
	return m_CreationOk;
}

void Texture::DrawFilledRect(const Rectf& rect) const
{
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	{
		glVertex2f(rect.left, rect.bottom);
		glVertex2f(rect.left + rect.width, rect.bottom);
		glVertex2f(rect.left + rect.width, rect.bottom + rect.height);
		glVertex2f(rect.left, rect.bottom + rect.height);
	}
	glEnd();
}
