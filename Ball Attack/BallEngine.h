#pragma once

#ifdef BALLENGINE_EXPORTS 
#define API _declspec(dllexport)
#else 
#define API _declspec(dllimport)
#endif

#include <cstdlib>

constexpr float scr_width = 1000.0f;
constexpr float scr_height = 800.0f;

constexpr int DLL_OK = 10001;
constexpr int DLL_ERR = 10002;
constexpr int DLL_NaN = 10003;

enum class dirs { stop = 0, up = 1, down = 2, left = 3, right = 4, up_left = 5, up_right = 6, down_left = 7, down_right = 8 };
enum class colors { blue = 0, red = 1, green = 2, purple = 3, yellow = 4 };
enum class types { egg = 0, ball = 1, catapult = 2, axe = 3, no_type = 4 };
enum class sizes { small_ball = 0, middle = 1, big = 2 };

namespace dll
{
	class API ATOM
	{
		protected:
			float width = 0;
			float height = 0;

			dirs dir = dirs::stop;
			types type = types::no_type;


		public:
			float x = 0;
			float y = 0;
			float ex = 0;
			float ey = 0;

			ATOM(float first_x, float first_y, float first_width, float first_height)
			{
				x = first_x;
				y = first_y;
				width = first_width;
				height = first_height;
				ex = x + width;
				ey = y + height;
			}

			virtual ~ATOM() {};

			float GetWidth() const
			{
				return width;
			}
			float GetHeight() const 
			{
				return height;
			}
			dirs GetDir() const 
			{
				return dir;
			}
			types GetType() const
			{
				return type;
			}

			void SetWidth(float new_width)
			{
				width = new_width;
				ex = x + width;
			}
			void SetHeight(float new_height)
			{
				height = new_height;
				ey = y + height;
			}
			void SetDir(dirs new_dir)
			{
				dir = new_dir;
			}
			void SetTypes(types new_type)
			{
				type = new_type;
			}

			void NewDims(float new_width, float new_height)
			{
				width = new_width;
				height = new_height;
				ex = x + width;
				ey = y + height;
			}

			void SetEdges()
			{
				ex = x + width;
				ey = y + height;
			}
	};

	class API OBJECT :public ATOM
	{
		protected:
			float slope = 0;
			float intercept = 0;
			
			int max_frames = 3;
			int frame_delay = 10;
			int current_frame = 0;

			float speed = 1.0f;

			bool vert_move = false;

			void SetMoveVector(float start_x, float start_y, float end_x, float end_y)
			{
				if (end_x - start_x == 0)vert_move = true;
				else
				{
					vert_move = false;
					slope = (end_y - start_y) / (end_x - start_x);
					intercept = start_y - slope * start_x;
				}
			}

		public:
			sizes size = sizes::big;
			colors color = colors::blue;
			bool now_shooting = false;

			OBJECT(types what_type, float first_x, float first_y) :ATOM(first_x, first_y, 1.0f, 1.0f)
			{
				type = what_type;

				switch (type)
				{
				case types::catapult:
					NewDims(100.0f, 85.0f);
					break;

				case types::axe:
					NewDims(25.0f, 25.0f);
					break;

				case types::egg:
					NewDims(76.0f, 76.0f);
					switch (rand() % 4)
					{
					case 0:
						dir = dirs::up_left;
						SetMoveVector(first_x, first_y, first_x - rand() % 50, 50.0f);
						break;

					case 1:
						dir = dirs::up_right;
						SetMoveVector(first_x, first_y, first_x + rand() % 50, 50.0f);
						break;

					case 2:
						dir = dirs::down_left;
						SetMoveVector(first_x, first_y, first_x - rand() % 50, scr_height - 100.0f);
						break;

					case 3:
						dir = dirs::down_right;
						SetMoveVector(first_x, first_y, first_x + rand() % 50, scr_height - 100.0f);
						break;

					}
					color = static_cast<colors>(rand() % 5);
					break;

				case types::ball:
					NewDims(76.0f, 76.0f);
					switch (rand() % 4)
					{
					case 0:
						dir = dirs::up_left;
						SetMoveVector(first_x, first_y, first_x - rand() % 50, 50.0f);
						break;

					case 1:
						dir = dirs::up_right;
						SetMoveVector(first_x, first_y, first_x + rand() % 50, 50.0f);
						break;

					case 2:
						dir = dirs::down_left;
						SetMoveVector(first_x, first_y, first_x - rand() % 50, scr_height - 100.0f);
						break;

					case 3:
						dir = dirs::down_right;
						SetMoveVector(first_x, first_y, first_x + rand() % 50, scr_height - 100.0f);
						break;

					}
					color = static_cast<colors>(rand() % 5);
					break;
				}
			}

			virtual ~OBJECT() {};

			virtual void Release() = 0;
			virtual void Move(float _speed) = 0;
			virtual int Transform(sizes to_what) = 0;
			virtual int GetFrame() = 0;
	};

	typedef ATOM* Atom;
	typedef OBJECT* Object;

	extern API Object Factory(types what, float s_x, float s_y);
	extern API Object AxeFactory(float s_x, float s_y, float e_x, float e_y);
}