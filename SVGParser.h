#pragma once
#include <string>
#include <vector>

class SVGParser final
{
public:
	// The only function to be called
	static bool GetVerticesFromSvgFile(const std::string& filePath, std::vector<std::vector<Point2f>>& vertices);

private:
	//static bool LoadGeometryFromSvgStream(unsigned char* pBlob, int blobSize, std::vector<Point2f> &vertices);
	static void RemoveSpaces(std::string& svgString);
	static bool GetVerticesFromSvgString(std::string& svgText, std::vector<std::vector<Point2f>>& vertices);
	static bool GetVerticesFromPathData(const std::string& pathData, std::vector<Point2f>& vertices);
	static bool GetElementContent(const std::string& svgText, const std::string& elementName, std::string& elementValue, size_t& startContentPos, size_t& endContentPos);
	static bool GetAttributeValue(const std::string& svgText, const std::string& attributeName, std::string& attributeValue);

	// Skips any optional commas in the stream
	// SVG has a really funky format,
	// not sure this code works for all cases.
	// TODO: Test cases!
	static void SkipSvgComma(std::stringstream& stream, bool isRequired);
	static float ReadSvgValue(std::stringstream& stream, float defaultValue);
	static float ReadSvgValue(std::stringstream& stream, bool separatorRequired);

	// Reads a single point
	static Point2f ReadSvgPoint(std::stringstream& stream);

	// Read the first point,
	// taking into account relative and absolute positioning.
	// Stores this point, needed when path is closed
	// Advances the cursor if requested.
	static Point2f FirstSvgPoint(std::stringstream& stream, Point2f& cursor, char cmd, bool isOpen, bool advance);

	// Read the next point,
	// taking into account relative and absolute positioning.
	// Advances the cursor if requested.
	// Throws an exception if the figure is not open
	static Point2f NextSvgPoint(std::stringstream& stream, Point2f& cursor, char cmd, bool isOpen, bool advance);

	// Reads next point, given only the new x coordinate
	static Point2f NextSvgCoordX(std::stringstream& stream, Point2f& cursor, char cmd, bool isOpen);

	// Reads next point, given only the new y coordinate
	static Point2f NextSvgCoordY(std::stringstream& ssRef, Point2f& cursor, char cmd, bool isOpen);
};
