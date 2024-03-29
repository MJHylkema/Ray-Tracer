/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The color class
*  A simple colour class with a set of operations including
*    phong lighting.
-------------------------------------------------------------*/

#ifndef H_COLOR
#define H_COLOR

class Color
{

public:
    float r, g, b;
	static const Color WHITE;
	static const Color BLACK;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color LIGHTBLUE;
	static const Color YELLOW;
	static const Color LIGHTYELLOW;
	static const Color GRAY;
	static const Color PURPLE;
	static const Color LIGHTPURPLE;
	static const Color ORANGE;

    Color()
		: r(1), g(1), b(1)
	{}	;
	
    Color(float rCol, float gCol, float bCol)
		: r(rCol), g(gCol), b(bCol)
	{} ;

    void scaleColor(float scaleFactor);

    void combineColor(Color col);

    void combineColor(Color col, float scaleFactor);

	Color phongLight(Color ambientCol, float diffuseTerm,  float specularTerm);
};
#endif
