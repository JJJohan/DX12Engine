namespace EngineNET
{
	static Vector2::Vector2()
	{
		Zero = gcnew Vector2();
		One = gcnew Vector2(1.0f, 1.0f);
		Up = gcnew Vector2(0.0f, 1.0f);
		Down = gcnew Vector2(0.0f, -1.0f);
		Left = gcnew Vector2(-1.0f, 0.0f);
		Right = gcnew Vector2(1.0f, 0.0f);
	}

	Vector2::Vector2()
	{
		_pVector2 = new Engine::Vector2();
	}

	Vector2::~Vector2()
	{
		this->!Vector2();
	}

	Vector2::!Vector2()
	{
		delete _pVector2;
	}

	Vector2::Vector2(Engine::Vector2& vec)
	{
		_pVector2 = new Engine::Vector2();
		_pVector2->X = vec.X;
		_pVector2->Y = vec.Y;
	}

#pragma managed(push, off)
	float* Operation(Engine::Vector2* lhs, Engine::Vector2* rhs, int op)
	{
		Engine::Vector2 out;
		switch (op)
		{
		case 0: // Add
			out = lhs->operator+(*rhs);
			break;
		case 1: // Sub
			out = lhs->operator-(*rhs);
			break;
		case 2: // Mul
			out = lhs->operator*(*rhs);
			break;
		case 3: // Div
			out = lhs->operator/(*rhs);
			break;
		}

		return new float[2]{ out.X, out.Y };
	}

	float* Operation(Engine::Vector2* lhs, float rhs, int op)
	{
		Engine::Vector2 out;
		switch (op)
		{
		case 0: // Mul
			out = lhs->operator*(rhs);
			break;
		case 1: // Div
			out = lhs->operator/(rhs);
			break;
		}

		return new float[2]{ out.X, out.Y };
	}
#pragma managed(pop)

	Vector2::Vector2(Vector2^ rhs)
	{
		Engine::Vector2* vec = rhs->_pVector2;
		_pVector2 = new Engine::Vector2(vec->X, vec->Y);
	}

	Vector2::Vector2(float x, float y)
	{
		_pVector2 = new Engine::Vector2(x, y);
	}

	Vector2::Vector2(int x, int y)
	{
		_pVector2 = new Engine::Vector2(x, y);
	}

	Vector2^ Vector2::operator+(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 0);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector2^ Vector2::operator*(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 2);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector2^ Vector2::operator-(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 1);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector2^ Vector2::operator/(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 3);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector2^ Vector2::operator*(float rhs)
	{
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		float* out = Operation(lhsVec, rhs, 0);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	Vector2^ Vector2::operator/(float rhs)
	{
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		float* out = Operation(lhsVec, rhs, 1);
		Vector2^ outVec = gcnew Vector2(out[0], out[1]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	void Vector2::operator=(Vector2^ rhs)
	{
		delete _pVector2;
		Engine::Vector2* vec = rhs->_pVector2;
		_pVector2 = new Engine::Vector2(vec->X, vec->Y);
	}

	Vector2::operator Engine::Vector2*()
	{
		return _pVector2;
	}

	Vector2^ Vector2::operator+=(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 0);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector2^ Vector2::operator*=(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 2);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector2^ Vector2::operator-=(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 1);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector2^ Vector2::operator/=(Vector2^ rhs)
	{
		Engine::Vector2* ref = rhs->_pVector2;
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		Engine::Vector2* rhsVec = new Engine::Vector2(ref->X, ref->Y);
		float* out = Operation(lhsVec, rhsVec, 3);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector2^ Vector2::operator*=(float rhs)
	{
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		float* out = Operation(lhsVec, rhs, 0);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;

		return this;
	}

	Vector2^ Vector2::operator/=(float rhs)
	{
		Engine::Vector2* lhsVec = new Engine::Vector2(_pVector2->X, _pVector2->Y);
		float* out = Operation(lhsVec, rhs, 1);
		X = out[0];
		Y = out[1];
		delete[] out;
		delete lhsVec;

		return this;
	}

	float Vector2::X::get()
	{
		return _pVector2->X;
	}

	void Vector2::X::set(float v)
	{
		_pVector2->X = v;
	}

	float Vector2::Y::get()
	{
		return _pVector2->Y;
	}

	void Vector2::Y::set(float v)
	{
		_pVector2->Y = v;
	}
}