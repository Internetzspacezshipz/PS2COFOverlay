#pragma once

using namespace std;

template<typename Type, int InitialSize = 5, int Slack = 5, int ReduceSize = Slack * 3>
class XDynamicArray
{
public:
	XDynamicArray()
	{
		XDynamicArray::XDynamicArray(InitialSize);
	}

	XDynamicArray(int initSize)
	{
		Size = initSize;
		Data = new Type[Size];
		for (int i = 0; i < Size; i++) {
			Data[i] = Type(); // Fill with zeroes
		}
	}

	XDynamicArray(const XDynamicArray<Type>& original)
	{
		Size = original.Size;
		Data = new Type[Size];
		for (int i = 0; i < Size; i++)
		{
			Data[i] = original.Data[i];
		}
	}

	~XDynamicArray()
	{
		delete[] Data;
	}

	void SetElement(int position, const Type& Value)
	{
		//ASSERT(position > Size - 1);
		Data[position] = Value;
	}

	void Add(const Type& Value)
	{
		if (Size == DataSize)
		{
			//Resize + slack
			Resize(DataSize);
		}
		Data[Size] = Value;
	}

	//Will shift array downward if halfway down.
	bool RemoveAt(int Location)
	{
		if (IsValidIndex(Location))
		{
			if (Location == Size - 1)
			{
				Size = Size - 1;
			}
			else
			{
				memcpy(&Data[Location], &Data[Location + 1], (Size - Location) * sizeof(Type));
			}
			MaybeResize();
			return true;
		}
		return false;
	}

	bool IsValidIndex(int Index) const
	{
		return Index >= 0 && Index < Size;
	}

	int Num() const
	{
		return Size;
	}

	Type& operator[](int index) 
	{
		//ASSERT(index > Size - 1);
		//Purposely do not resize.
		return Data[index];
	}

	const Type& operator[](int index) const
	{
		//ASSERT(index > Size - 1);
		//Purposely do not resize.
		return Data[index];
	}

	Type& begin()
	{
		return Data[0];
	}

	Type& end()
	{
		return Data[Size];
	}

private:
	//Maybe downsizes
	void MaybeResize()
	{
		if (DataSize - Size > ReduceSize)
		{
			Resize(Size);
		}
	}

	void Resize(int newSize)
	{
		Type* temp;
		temp = new Type[newSize + Slack];
		for (int i = 0; i < newSize; i++)
		{
			if (i < Size)
			{
				temp[i] = Data[i];
			}
			else
			{
				//Zero the rest of the mem
				ZeroMemory(&temp[i], sizeof(Type) * (newSize - i));
				break;
			}
		}
		delete[] Data;
		Data = temp;
		Size = newSize;
		DataSize = newSize + Slack;
	}


	int Size;
	int DataSize;
	Type* Data;
};
