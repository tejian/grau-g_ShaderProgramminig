#include "showdegree.h"

#include <QPainter>

#include <iostream>
#include <string>


void ShowDegree::onPluginLoad()
{
	glwidget()->makeCurrent();
	// Carregar shader, compile & link 
	vs = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vs->compileSourceFile("plugins/showdegree/showdegree.vert");

	fs = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fs->compileSourceFile("plugins/showdegree/showdegree.frag");

	program = new QOpenGLShaderProgram(this);
	program->addShader(vs);
	program->addShader(fs);
	program->link();

	this -> calculateInfo();
}


void ShowDegree::onObjectAdd()
{
	this -> calculateInfo();
}

void ShowDegree::calculateInfo()
{
	const Object& obj = scene() -> objects()[0];
	const std::vector <Vertex>& vertices = obj.vertices();
	std::vector <int> counts (vertices.size(), 0);

	for (int i = 0; i < obj.faces().size(); ++i)
	{
		const Face& face = obj.faces()[i];
		for (int j = 0; j < face.numVertices(); ++j)
		{
			++counts[face.vertexIndex (j)];
		}
	}
	
	unsigned int totalDegree = 0;
	for (int i = 0; i < vertices.size(); ++i)
		totalDegree += counts[i];

	migDegree = float (totalDegree) / vertices.size();
}

void drawRect(GLWidget &g);

void ShowDegree::postFrame()
{
	GLWidget &g=*glwidget();
	g.makeCurrent();
	const int SIZE = 1024;
	// 1. Create image with text
	QImage image(SIZE,SIZE,QImage::Format_RGB32);
	image.fill(Qt::white);    
	QPainter painter;
	painter.begin(&image);
	QFont font;
	font.setPixelSize(32);
	painter.setFont(font);
	painter.setPen(QColor(50,50,50));
	int x = 15;
	int y = 50;
	std::string info = "average Degree: " + std::to_string (migDegree);
	painter.drawText(x, y, info.c_str());    
	painter.end();

    // 2. Create texture
    const int textureUnit = 5;
    g.glActiveTexture(GL_TEXTURE0+textureUnit);
    QImage im0 = image.mirrored(false, true).convertToFormat(QImage::Format_RGBA8888, Qt::ColorOnly);
	g.glGenTextures( 1, &textureID);
	g.glBindTexture(GL_TEXTURE_2D, textureID);
	g.glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, im0.width(), im0.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, im0.bits());
	g.glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	g.glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    // Pass 2. Draw quad using texture
    program->bind();
    program->setUniformValue("colorMap", textureUnit);
    program->setUniformValue("WIDTH", float(glwidget()->width()));
    program->setUniformValue("HEIGHT", float(glwidget()->height()));    
 
    // quad covering viewport 
    drawRect(g);
    program->release();
    g.glBindTexture(GL_TEXTURE_2D, 0);

    g.glDeleteTextures(1, &textureID);

}

void drawRect(GLWidget &g)
{
    static bool created = false;
    static GLuint VAO_rect;

    // 1. Create VBO Buffers
    if (!created)
    {
        created = true;
        
        // Create & bind empty VAO
        g.glGenVertexArrays(1, &VAO_rect);
        g.glBindVertexArray(VAO_rect);

        float z = -0.99999;
        // Create VBO with (x,y,z) coordinates
        float coords[] = { -1, -1, z, 
                            1, -1, z, 
                           -1,  1, z, 
                            1,  1, z};

        GLuint VBO_coords;
        g.glGenBuffers(1, &VBO_coords);
        g.glBindBuffer(GL_ARRAY_BUFFER, VBO_coords);
        g.glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
        g.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        g.glEnableVertexAttribArray(0);
        g.glBindVertexArray(0);
    }

    // 2. Draw
    g.glBindVertexArray (VAO_rect);
    g.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    g.glBindVertexArray(0);
}
#if QT_VERSION < 0x050000                                                                                                                         
Q_EXPORT_PLUGIN2(showdegree, ShowDegree)   // plugin name, plugin class
#endif 
