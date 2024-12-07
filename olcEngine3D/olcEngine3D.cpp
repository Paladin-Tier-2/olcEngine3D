 #include "olcConsoleGameEngine.h"
using namespace std;

struct vec3d
{
	float x, y, z;
};

struct triangle {
	vec3d p[3];
};


struct mesh {

	vector<triangle> tris;
};

struct mat4x4
{
	// m[ROW][COL]
	float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine
{

public:
	olcEngine3D()
	{
		m_sAppName = L"3D Demo";
	}


private:
	mesh meshCube;
	mat4x4 matProj;
	float fTheta;

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
	{
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0]; 
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

		if (w != 0) {
			o.x /= w;
			o.y /= w;
			o.z /= w; // NOT sure about this one
		}
	}

public:
	bool OnUserCreate() override

	{
		meshCube.tris = {
			// South face
			{ { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f} } },
			{ { {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f} } },

			// East face
			{ { {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f} } },
			{ { {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f} } },

			// North face
			{ { {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f} } },
			{ { {1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} } },

			// West face
			{ { {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} } },
			{ { {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f} } },

			// Top face
			{ { {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f} } },
			{ { {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f} } },

			// Bottom face
			{ { {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f} } },
			{ { {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} } },
		};

		// Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1 / (  tanf(  0.5f * fFov * (3.14159f / (180.0f)) )  );
		float q = fFar / (fFar - fNear);
		   
		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = q;
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = fNear * q;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Background
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		mat4x4 matRotX, matRotZ;
		fTheta += 1.0f * fElapsedTime;

		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta);
		matRotX.m[1][2] = sinf(fTheta);
		matRotX.m[2][1] = -sinf(fTheta);
		matRotX.m[2][2] = cosf(fTheta);
			   
		// They will be biased differently -- hence multiplied by 1/2 

		matRotZ.m[0][0] = cosf(fTheta * 0.5f);
		matRotZ.m[1][1] = cosf(fTheta * 0.5f);
		matRotZ.m[0][1] = -sinf(fTheta * 0.5f);
		matRotZ.m[1][0] = sinf(fTheta * 0.5f);
		matRotZ.m[2][2] = 1; 





		// Draw Trianlgle
		for (auto tri : meshCube.tris) {

			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate by z-axis
			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);


			// Rotate by x-axis now
			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);



			// Order of transformation matter! 
			// Now take the final rotated transformation 

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;


             // Normalized Screen Space
			// Project traingles ( FROM 3D to 2D ) 
			MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
			MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
			MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);


			// Scaling it to view - Still Normalized Screen Space
			triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
			triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
			triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

			// Console Space
			triProjected.p[0].x *= 0.5f * (float)ScreenWidth(); triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[1].x *= 0.5f * (float)ScreenWidth(); triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			triProjected.p[2].x *= 0.5f * (float)ScreenWidth(); triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

			// Now it is between 0 and 2, and scale it by appropriate axis axis 
			// However, our cube's origin is located at 0 and our view into the world is located at 0 - our face is probably
			// inside the cube. That means some of the cube is behind and we are still trying to draw it.
			// What we need to do is translate the cube into the world away from the camera


			DrawTriangle
			(
				triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID,FG_WHITE
			);
		}
		return true;
	}
};

int main() {

	olcEngine3D demo;
	if (demo.ConstructConsole(256, 240, 2, 2) )
		demo.Start();

	
	return 0;
}



