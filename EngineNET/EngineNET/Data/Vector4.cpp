namespace EngineNET
{
	static Vector4::Vector4()
	{
		Zero = gcnew Vector4();
		One = gcnew Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	Vector4::Vector4()
	{
		_pVector4 = new Engine::Vector4();
	}

	Vector4::~Vector4()
	{
		this->!Vector4();
	}

	Vector4::!Vector4()
	{
		delete _pVector4;
	}

	Vector4::Vector4(Vector3^ rhs)
	{
		if (_pVector4 != nullptr)
		{
			delete _pVector4;
		}

		_pVector4->X = rhs->X;
		_pVector4->Y = rhs->Y;
		_pVector4->Z = rhs->Z;
		_pVector4->W = 0.0f;
	}

	Vector4::Vector4(Engine::Vector4& vec)
	{
		_pVector4 = new Engine::Vector4();
		_pVector4->X = vec.X;
		_pVector4->Y = vec.Y;
		_pVector4->Z = vec.Z;
		_pVector4->W = vec.W;
	}

#pragma managed(push, off)
	float* Operation(Engine::Vector4* lhs, Engine::Vector4* rhs, int op)
	{
		Engine::Vector4 out;
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

		return new float[4]{ out.X, out.Y, out.Z, out.W };
	}

	float* Operation(Engine::Vector4* lhs, float rhs, int op)
	{
		Engine::Vector4 out;
		switch (op)
		{
		case 0: // Mul
			out = lhs->operator*(rhs);
			break;
		case 1: // Div
			out = lhs->operator/(rhs);
			break;
		}

		return new float[4]{ out.X, out.Y, out.Z, out.W };
	}
#pragma managed(pop)

	Vector4::Vector4(Vector4^ rhs)
	{
		Engine::Vector4* vec = rhs->_pVector4;
		_pVector4 = new Engine::Vector4(vec->X, vec->Y, vec->Z, vec->W);
	}

	Vector4::Vector4(float x, float y, float z, float w)
	{
		_pVector4 = new Engine::Vector4(x, y, z, w);
	}

	Vector4::Vector4(int x, int y, int z, int w)
	{
		_pVector4 = new Engine::Vector4(x, y, z, w);
	}

	Vector4^ Vector4::operator+(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 0);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector4^ Vector4::operator*(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 2);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector4^ Vector4::operator-(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 1);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector4^ Vector4::operator/(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 3);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		delete rhsVec;
		return outVec;
	}

	Vector4^ Vector4::operator*(float rhs)
	{
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		float* out = Operation(lhsVec, rhs, 0);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	Vector4^ Vector4::operator/(float rhs)
	{
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		float* out = Operation(lhsVec, rhs, 1);
		Vector4^ outVec = gcnew Vector4(out[0], out[1], out[2], out[3]);
		delete[] out;
		delete lhsVec;
		return outVec;
	}

	void Vector4::operator=(Vector4^ rhs)
	{
		delete _pVector4;
		Engine::Vector4* vec = rhs->_pVector4;
		_pVector4 = new Engine::Vector4(vec->X, vec->Y, vec->Z, vec->W);
	}

	Vector4::operator Engine::Vector4*()
	{
		return _pVector4;
	}

	Vector4^ Vector4::operator+=(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 0);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector4^ Vector4::operator*=(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 2);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector4^ Vector4::operator-=(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 1);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector4^ Vector4::operator/=(Vector4^ rhs)
	{
		Engine::Vector4* ref = rhs->_pVector4;
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		Engine::Vector4* rhsVec = new Engine::Vector4(ref->X, ref->Y, ref->Z, ref->W);
		float* out = Operation(lhsVec, rhsVec, 3);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;
		delete rhsVec;

		return this;
	}

	Vector4^ Vector4::operator*=(float rhs)
	{
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		float* out = Operation(lhsVec, rhs, 0);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;

		return this;
	}

	Vector4^ Vector4::operator/=(float rhs)
	{
		Engine::Vector4* lhsVec = new Engine::Vector4(_pVector4->X, _pVector4->Y, _pVector4->Z, _pVector4->W);
		float* out = Operation(lhsVec, rhs, 1);
		X = out[0];
		Y = out[1];
		Z = out[2];
		W = out[3];
		delete[] out;
		delete lhsVec;

		return this;
	}

	float Vector4::X::get()
	{
		return _pVector4->X;
	}

	void Vector4::X::set(float v)
	{
		_pVector4->X = v;
	}

	float Vector4::Y::get()
	{
		return _pVector4->Y;
	}

	void Vector4::Y::set(float v)
	{
		_pVector4->Y = v;
	}

	float Vector4::Z::get()
	{
		return _pVector4->Z;
	}

	void Vector4::Z::set(float v)
	{
		_pVector4->Z = v;
	}

	float Vector4::W::get()
	{
		return _pVector4->W;
	}

	void Vector4::W::set(float v)
	{
		_pVector4->W = v;
	}
}