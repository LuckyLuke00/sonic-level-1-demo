#include "pch.h"
#include "SVGParser.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

bool SVGParser::GetVerticesFromSvgFile(const std::string& filePath, std::vector<std::vector<Point2f>>& vertices)
{
	// Open the file
	std::ifstream svgStream(filePath.c_str());

	if (!svgStream)
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), failed to load vertices from file " << filePath << std::endl;
		return false;
	}

	std::string svgString;
	while (!svgStream.eof())
	{
		std::string svgLine;
		getline(svgStream, svgLine);
		svgString += svgLine;
	}

	// close the file
	svgStream.close();

	// Cleanup
	RemoveSpaces(svgString);

	if (!GetVerticesFromSvgString(svgString, vertices))
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), malformed or unsupported information in file " << filePath << std::endl;
		return false;
	}

	// Get the viewbox rect to flip the y coordinates. SVG has org topleft, the framework bottom left.
	std::string viewBoxValue;
	if (!GetAttributeValue(svgString, "viewBox", viewBoxValue))
	{
		std::cerr << "SVGParser::GetVerticesFromSvgFile(..), no viewbox information found in " << filePath << std::endl;;
		vertices.clear();
		return false;
	}

	Rectf viewBox{};
	std::stringstream sstream{ viewBoxValue };
	sstream >> viewBox.left >> viewBox.bottom >> viewBox.width >> viewBox.height;

	//std::vector<std::vector<Point2f>> vertices{ vertices };
	for (size_t i{}; i < vertices.size(); ++i)
	{
		// flip the y coordinate
		for (Point2f& p : vertices[i])
		{
			p.y = viewBox.height - p.y;
		}
	}

	return true;
}

void SVGParser::RemoveSpaces(std::string& svgString)
{
	// Remove spaces before and = chars
	size_t foundPos{};
	while ((foundPos = svgString.find(" =")) != std::string::npos)
	{
		svgString.replace(foundPos, 2, "=");
	}
	// Remove spaces after and = chars
	while ((foundPos = svgString.find("= ")) != std::string::npos)
	{
		svgString.replace(foundPos, 2, "=");
	}
	//std::cout << svgString.size( ) << "\n";

	// Remove spaces before and > chars
	while ((foundPos = svgString.find(" >")) != std::string::npos)
	{
		svgString.replace(foundPos, 2, ">");
	}
	// Remove spaces after and < chars
	while ((foundPos = svgString.find("< ")) != std::string::npos)
	{
		svgString.replace(foundPos, 2, "<");
	}
	//std::cout << svgString << "\n";
}

bool SVGParser::GetVerticesFromSvgString(std::string& svgString, std::vector<std::vector<Point2f>>& vertices)
{
	size_t startPosContent{};
	size_t endPosContent{};

	std::string pathElementContent;

	// Get path element until none has been found anymore
	while (GetElementContent(svgString, "path", pathElementContent, startPosContent, endPosContent))
	{
		// Vector of Point2f to fill with a path's vertices
		std::vector<Point2f> verticesVector;

		// Get d attribute value
		std::string pathDataValue{};
		if (!GetAttributeValue(pathElementContent, " d", pathDataValue))
		{
			std::cerr << "SVGParser::GetVerticesFromSvgString(..), path element doesn't contain a d-attribute.\n ";
			vertices.clear();
			return false;
		}
		// Process the path data
		if (!GetVerticesFromPathData(pathDataValue, verticesVector))
		{
			std::cerr << "SVGParser::GetVerticesFromSvgString(..), error while extracting vertices from the path. \n";
			vertices.clear();
			return false;
		}

		if (verticesVector.size() == 0)
		{
			std::cerr << "Empty verticesVector in GetVerticesFromSvgString(..), no vertices found in the path element" << std::endl;
			vertices.clear();
			return false;
		}

		// DEBUG: Read vertices of current vector
		//for (Point2f& p : verticesVector)
		//{
		//	std::cout << p.x << " " << p.y << std::endl;
		//}

		// Add the vector to the vector array
		vertices.push_back(verticesVector);
	}

	if (vertices.size() == 0)
	{
		std::cerr << "Empty vertices in GetVerticesFromSvgString(..), no path element(s) found" << std::endl;
		return false;
	}

	return true;
}

bool SVGParser::GetVerticesFromPathData(const std::string& pathData, std::vector<Point2f>& vertices)
{
	const std::string pathCmdChars(("mMZzLlHhVvCcSsQqTtAa"));

	// Use streamstream for parsing
	std::stringstream ss(pathData);

	char cmd{ 0 };
	Point2f cursor{};
	Point2f startPoint{};//At the end of the z command, the new current point is set to the initial point of the current subpath.

	bool isOpen = true;

	// http://www.w3.org/TR/SVG/paths.html#Introduction
	Point2f vertex{};
	char pathCommand{};
	ss >> pathCommand;
	while (!ss.eof())
	{
		//if ( strchr( pathCmdChars.c_str( ), pathCommand ) != 0 )
		// if the command is a valid command letter, proceed
		if (pathCmdChars.find(pathCommand) != std::string::npos)
		{
			cmd = pathCommand;
		}
		else
		{
			// if not a command, then put it back
			// Attempts to decrease the current location in the stream by one character,
			// making the last character extracted from the stream once again available to be extracted by input operation
			ss.putback(pathCommand);
		}

		switch (cmd)
		{
		case ('Z'):
		case ('z'):
			isOpen = true;
			break;

		case ('M'):
		case ('m'):
			if (isOpen)
			{
				cursor = FirstSvgPoint(ss, cursor, cmd, isOpen, true);
				startPoint = cursor;
				vertices.push_back(cursor);
				isOpen = false;
				break;
			}
			// Fallthrough when isOpen
		case ('L')://lineto
		case ('l'):
			vertex = NextSvgPoint(ss, cursor, cmd, isOpen, true);
			vertices.push_back(vertex);
			break;

		case ('h'): // horizontal lineto
		case ('H'):
			vertex = NextSvgCoordX(ss, cursor, cmd, isOpen);
			vertices.push_back(vertex);
			break;

		case ('v'): // vertical lineto
		case ('V'):
			vertex = NextSvgCoordY(ss, cursor, cmd, isOpen);
			vertices.push_back(vertex);
			break;

		case ('C'):
		case ('c'):
			std::cerr << "SVGParser::GetVerticesFromPathData,  beziers are not supported.\nHave another look at the guide, or select all nodes in inkscape and press shift + L\n";
			return false;

		default:
			std::cerr << "SVGParser::GetVerticesFromPathData, " << cmd << " is not a supported SVG command";
			return false;
		}
		// Next command
		ss >> pathCommand;
	}

	return true;
}

bool SVGParser::GetElementContent(const std::string& svgText, const std::string& elementName, std::string& elementContent, size_t& startContentPos, size_t& endContentPos)
{
	// 2 possible formats
	// <ElementName> content <ElementName/>

	// Temporary start and end positions for checking
	auto tempStartPos{ startContentPos };
	auto tempEndPos{ endContentPos };

	std::string startElement = "<" + elementName + ">";
	std::string endElement = "<" + elementName + "/>";
	if ((tempStartPos = svgText.find(startElement)) != std::string::npos)
	{
		tempStartPos += startElement.length();
		if ((tempEndPos = svgText.find(endElement)) != std::string::npos)
		{
			elementContent = svgText.substr(tempStartPos, tempEndPos - tempStartPos);
			startContentPos = tempStartPos;
			endContentPos = tempEndPos;
			return true;
		}
		else
		{
			return false;
		}
	}

	// or
	// <ElementName content />

	tempStartPos = startContentPos;
	tempEndPos = endContentPos;

	startElement = "<" + elementName;
	endElement = "/>";
	if ((tempStartPos = svgText.find(startElement, tempStartPos)) != std::string::npos)
	{
		tempStartPos += startElement.length();
		if ((tempEndPos = svgText.find(endElement)) != std::string::npos)
		{
			elementContent = svgText.substr(tempStartPos, tempEndPos - tempStartPos);
			startContentPos = tempStartPos;
			endContentPos = tempEndPos;
			return true;
		}
	}
	return false;
}

bool SVGParser::GetAttributeValue(const std::string& svgText, const std::string& attributeName, std::string& attributeValue)
{
	const std::string searchAttributeName{ attributeName + "=" };

	const auto attributePos = svgText.find(searchAttributeName);
	if (attributePos == std::string::npos)
	{
		return false;
	}

	const auto openingDoubleQuotePos{ svgText.find("\"", attributePos) };
	if (openingDoubleQuotePos == std::string::npos)
	{
		return false;
	}

	const auto closingDoubleQuotePos{ svgText.find("\"", openingDoubleQuotePos + 1) };
	if (closingDoubleQuotePos == std::string::npos)
	{
		return false;
	}

	attributeValue = svgText.substr(openingDoubleQuotePos + 1, closingDoubleQuotePos - openingDoubleQuotePos - 1);
	//std::cout << attributeName << ":" << attributeValue << "\n";
	return true;
}

// Skips any optional commas in the stream
// SVG has a really funky format,
// not sure this code works for all cases.
// TODO: Test cases!
void SVGParser::SkipSvgComma(std::stringstream& svgStream, bool isRequired)
{
	while (true)
	{
		const char c = static_cast<char>(svgStream.get());

		if (svgStream.eof())
		{
			if (isRequired)
			{
				std::cerr << "SVGParser::SkipSvgComma, expected comma or whitespace\n";
			}
			break;
		}

		if (c == (','))
			return;

		if (!isspace(c))
		{
			svgStream.unget();
			return;
		}
	}
}

float SVGParser::ReadSvgValue(std::stringstream& svgStream, float defaultValue)
{
	float s{};
	svgStream >> s;

	if (svgStream.eof())
	{
		s = defaultValue;
	}
	else
	{
		SkipSvgComma(svgStream, false);
	}

	return s;
}

float SVGParser::ReadSvgValue(std::stringstream& svgStream, bool separatorRequired)
{
	float s{};
	svgStream >> s;
	SkipSvgComma(svgStream, separatorRequired);
	return s;
}

// Reads a single point
Point2f SVGParser::ReadSvgPoint(std::stringstream& svgStream)
{
	//std::cout << "ReadSvgPoint: "  << svgStream.str() << "\n";
	Point2f p{};
	p.x = ReadSvgValue(svgStream, true);
	p.y = ReadSvgValue(svgStream, false);
	return p;
}

Point2f SVGParser::FirstSvgPoint(std::stringstream& svgStream, Point2f& cursor, char cmd, bool isOpen, bool advance)
{
	if (!isOpen)
	{
		std::cerr << "SVGParser::FirstSvgPoint, expected 'Z' or 'z' command";
	}

	Point2f p = ReadSvgPoint(svgStream);

	if (islower(cmd))
	{
		// Relative point
		p.x = cursor.x + p.x;
		p.y = cursor.y + p.y;
	}

	if (advance)
	{
		cursor = p;
	}

	return p;
}
// Read the next point,
// taking into account relative and absolute positioning.
// Advances the cursor if requested.
// Throws an exception if the figure is not open
Point2f SVGParser::NextSvgPoint(std::stringstream& svgStream, Point2f& cursor, char cmd, bool isOpen, bool advance)
{
	if (isOpen)
	{
		std::cerr << "SVGParser::NextSvgPoint, expected 'M' or 'm' command\n";
	}

	Point2f p = ReadSvgPoint(svgStream);

	if (islower(cmd))
	{
		// Relative point
		p.x = cursor.x + p.x;
		p.y = cursor.y + p.y;
	}

	if (advance)
	{
		cursor = p;
	}

	return p;
}

// Reads next point, given only the new x coordinate
Point2f SVGParser::NextSvgCoordX(std::stringstream& svgStream, Point2f& cursor, char cmd, bool isOpen)
{
	if (isOpen)
	{
		std::cerr << "SVGParser::NextSvgCoordX, expected 'M' or 'm' command\n";
	}

	float c;
	svgStream >> c;

	if (islower(cmd))
	{
		// Relative point
		cursor.x += c;
	}
	else
	{
		cursor.x = c;
	}

	return cursor;
}

// Reads next point, given only the new y coordinate
Point2f SVGParser::NextSvgCoordY(std::stringstream& svgStream, Point2f& cursor, char cmd, bool isOpen)
{
	if (isOpen)
	{
		std::cerr << "SVGParser::NextSvgCoordY, expected 'M' or 'm' command\n";
	}

	float c{};
	svgStream >> c;

	if (islower(cmd))
	{
		// Relative point
		cursor.y += c;
	}
	else
	{
		cursor.y = c;
	}

	return cursor;
}
