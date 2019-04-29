#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;
const float PI = 3.14159;

// Menu checks
char checkMenuInput(string input, string menu)
{
	bool check = 0;

	if (input.length() != 1)
	{
		throw string("    -- Invalid choice -- ");
	}
	for (int i = 0; i < menu.length(); i++)
	{
		if (input[0] == menu[i])
			check = 1;
	}
	if (check != 1)
	{
		throw string("    -- Invalid choice -- ");
	}
}

char select(string request, string menu)
{
	string input;
	bool check;

	do {
		cout << endl << request;
		getline(cin, input);

		check = 0;

		try
		{
			checkMenuInput(input, menu);
		}
		catch (string e)
		{
			cout << e << endl;
			check = 1;
		}

	} while (check);

	return input[0];
}

//Value checks
float spaceCheck(string s)
{
	size_t end_pos;
	float num = stof(s, &end_pos);

	for (size_t i = end_pos; i < s.length(); ++i)
	{
		if (!isspace(s[i]))
			throw string("    --Invalid input-- ");
	}
	if (num <= 0)
	{
		throw string("    --Number must be above 0 -- ");
	}
	return num;
}

string numCheck(string s)
{
	int i = 0;

	if (s[0] == '-')
	{
		throw string("    --Number must be above 0 -- ");
	}

	while (s[i])
	{
		if (isdigit(s[i]))
		{
			return s;
		}
		else {
			throw string("    --Invalid input-- ");
		}
		i++;
	}
}

float getValue(string request)
{
	string s;
	float value;
	
	for (;;)
	{
		try
		{
			cout << request;
			cin.seekg(0, ios::end);
			cin.clear();
			getline(cin, s);
			if (s.empty())
			{
				throw string("    --You must enter a value-- ");
			}
			s = numCheck(s);
			value = spaceCheck(s);

			return value;
		}
		catch (string e)
		{
			cout<< e  << endl << endl;
		}
	}
}

// Classes
class Circuit
{
protected:
	float gain;
public:
	static void Menu(void);
};

// AMPLIFIERS
class Amplifier : public Circuit
{
protected:
	float Vin, Vout, Rin, Rf;
public:
	Amplifier() 
	{
		cout << endl;
		Vin = getValue("Enter input voltage in volts ==>  ");
		Rin = getValue("Enter resistance of Rin in ohms ==>  ");
		Rf = getValue("Enter resistance of Rf in ohms ==>  ");
	}

	Amplifier(int num)
	{
		if (num == 1) 
		{	static int count = 1;

			cout << endl << "For voltage #" << count << " : " << endl;
			Vin = getValue("Enter input voltage in volts ==>  ");
			Rin = getValue("Enter resistance of Rin in ohms ==>  ");
			count++;
			
			if (count == 3)
			count = 1;
		}
		else if (num == 2)
		{
			cout << endl;
			Rf = getValue("Enter resistance of Rf in ohms ==>  ");
		}
	}

	void displayGain()
	{
		cout << endl << "Voltage gain = " << gain;
	}

	void findVout()
	{
		Vout = gain * Vin;
		cout << endl << "Vout = " << Vout << " V";
	}

	void findsumInfo(Amplifier& A, Amplifier& B)
	{
		gain = ((A.Vin/A.Rin) + (B.Vin/B.Rin))*(-1);
		Vout = Rf * gain;
		displayGain();
		cout << endl << "Vout = " << Vout << " V";
		
	}

	static void Menu(void);
};

class inAmp : public Amplifier
{
public:
	inAmp()
	{
		gain = ((Rf / Rin) * (-1));
		displayGain();
		findVout();
	}
};

class nonInAmp : public Amplifier
{
public:
	nonInAmp()
	{
		gain = (1 + (Rf / Rin));
		displayGain();
		findVout();
	}
};

class sumAmp: public Circuit
{
private:
	Amplifier a1, a2, total;
public:
	sumAmp(Amplifier in1, Amplifier in2, Amplifier in3): a1(in1), a2(in2), total(in3)
	{
		total.findsumInfo(a1, a2);
	}
};

//FILTERS
class Filter : public Circuit
{
protected:
	float res, cap, ind, fc;
public:
	static void Menu(void);
	static void pickTypeof(string e, int num);

	friend ostream& operator << (ostream& cout, Filter& f)
	{
		cout << "The cutoff frequency is " << f.fc << " Hz.";
		return cout;
	}

	bool operator > (Filter& f)
	{
		if (this->fc > f.fc)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool operator == (Filter& f)
	{
		if (this->fc == f.fc)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	friend class BPS;
};

class RC : public Filter
{ 
public:
	RC()
	{
		cout << endl << "RC Filter:" << endl;
		res = getValue("Enter resistance in Ohms == >  ");
		cap = getValue("Enter capacitance in uF ==>  ");
		cap /= 1000000;

		fc = 1 / (2 * PI * cap * res);
	}
	void calcHPAv(float freq)
	{ 
		float w = (2 * PI * freq);
		gain = (1 / sqrt(1+ pow((1/(w*cap*res)), 2)));
	}
	void calcLPAv(float freq)
	{
		float w = (2 * PI * freq);
		gain = (1 / (sqrt(1 + pow((w*cap*res), 2))));
	}
};

class RL : public Filter
{
public:
	RL()
	{
		cout << endl << "RL Filter: " << endl;
		res = getValue("Enter resistance in Ohms == >  ");
		ind = getValue("Enter inductance in mH ==>  ");
		ind /= 1000;

		fc = res / (2 * PI * ind);
	}
	void calcHPAv(float freq)
	{
		float w = (2 * PI * freq);
		gain = (1 / sqrt(1 + pow((res/(w*ind)), 2)));
	}
	void calcLPAv(float freq)
	{
		float w = (2 * PI * freq);
		gain = (1 / (sqrt(1 + pow(((w*ind)/res), 2))));
	}
};

class BPS 
{
private: 
	Filter HP, LP;
	float fL, fH;
public:
	static void BP(void);
	static void BS(void);
	void pickType(void);
	void checkBPFilter(void);
	void checkBSFilter(void);
};

// RESONANCE CIRCUITS
class Reson: public Filter
{
protected:
	float Q, BW;
public:
	static void Menu(void);
	Reson()
	{
		cout << endl ;
		res = getValue("Enter resistance in Ohms == >  ");
		cap = getValue("Enter capacitance in uF ==>  ");
		cap /= 1000000;
		ind = getValue("Enter inductance in mH ==>  ");
		ind /= 1000;

		fc = (1 / (2 * PI * sqrt(ind*cap)));		// fc used as fres
	}
	void displayInfo(void)
	{	
		cout << "Resonance frequency: " << fc << " Hz" << endl;
		cout << "Quality Factor is: " << Q << endl;
		cout << "BW of circuit is: " << BW << " Hz";
	}
};

class sRes : public Reson
{
public:
	sRes()
	{
		Q = ( 1 / res )* ( sqrt(ind / cap) );
		BW = fc / Q;
		cout << endl << "Series Resonance Circuit:  " << endl;
		displayInfo();
	}
};

class pRes : public Reson
{
public:
	pRes()
	{
		Q = ( res * (sqrt(cap / ind)));
		BW = fc / Q;
		cout << endl << "Parallel Resonance Circuit:  " << endl;
		displayInfo();
	}
};

// Class menus & Selection
void Circuit::Menu(void)
{
	char selection;

	system("cls");
	cout << "Circuits Calculator" << endl;
	cout << endl << "1. Operational Amplifiers" << endl;
	cout << "2. Passive Filters" << endl;
	cout << "3. Resonance Circuits" << endl;
	cout << "4. Exit" << endl;

	selection = select(string("Select circuit category ==> "), string("1234"));

	switch (selection)
	{
	case '1':
		Amplifier::Menu();
		break;
	case '2':
		Filter::Menu();
		break;
	case '3':
		Reson::Menu();
		break;
	case '4':
		cout << endl << "Exit Program : ";
		system("pause");
		exit(2);
	}
}

void Amplifier::Menu(void)
{
	char amp;

	system("cls");	
	cout <<  "Amplifiers Calculator" << endl;
	cout << endl << "Program will calculate gain and output voltage of diffferent amplifiers.";
	cout << endl << "A negative gain / voltage represents inversion. " << endl;

	cout << endl << "A. Inverting Amplifiers" << endl;
	cout << "B. Non-inverting Amplifiers" << endl;
	cout << "C. Summing Amplifiers" << endl;

	amp = select(string("Select amplifier type ==> "), string("AaBbCc"));

	if (amp == 'A' || amp == 'a')
	{
		inAmp circuit;
	}
	else if (amp == 'B' || amp == 'b')
	{
		nonInAmp circuit;
	}
	else
	{
		system("cls");
		cout << "Summing Amplifiers" << endl;
		Amplifier A1(1), A2(1), total(2);
		sumAmp circuit(A1, A2, total);
	}

	cout << endl << endl;
	system("pause");
}

void Filter::Menu(void)
{
	char filt;

	system("cls");
	cout << "Filters Calculator" << endl;
	cout << endl << "A. High Pass Filters" << endl;
	cout << "B. Low Pass Filters" << endl;
	cout << "C. Band-Pass Filters" << endl;
	cout << "D. Band-Stop Filters" << endl;

	filt = select(string("Select filter type ==> "), string("AaBbCcDd"));

	system("cls");

	switch (filt)
	{
	case 'a':
	case 'A':
		cout << "High Pass Filters ";
		pickTypeof("High Pass", 1);
		break;
	case 'b':
	case 'B':
		cout << "Low Pass Filters";
		pickTypeof("Low Pass", 0);
		break;
	case 'c':
	case 'C':
		BPS::BP();
		break;
	case 'd':
	case 'D':
		BPS::BS();
		break;
	}

	cout << endl << endl;
	system("pause");
}

void Reson::Menu(void)
{
	char reso;

	system("cls");
	cout << "Resonance Circuit Calculator" << endl;
	cout << endl << "All circuits calculated as ideal resonance." << endl;
	cout << endl << "A. Series Resonance" << endl;
	cout << "B. Parallel Resonance" << endl;

	reso= select(string("Select amplifier type ==> "), string("AaBb"));

	if (reso == 'A' || reso == 'a')
	{
		sRes circuit;
	}
	else if (reso == 'B' || reso == 'b')
	{
		pRes circuit;
	}

	cout << endl << endl;
	system("pause");
}

// Filter Functions
void BPS::BP(void)
{
	cout << "Band-Pass Filter " << endl;
	cout << "A band-pass filter can be made by placing a low pass filter after a high pass filter." << endl;
	cout << "The fc of the low pass filter must be higher than the fc of the high pass filter. " << endl;

	BPS BPfilter;
	BPfilter.pickType();
	BPfilter.checkBPFilter();
}

void BPS::BS(void)
{
	cout << "Band-Stop Filter " << endl;
	cout << "A band-stop filter can be made by placing a high pass filter after a low pass filter." << endl;
	cout << "The fc of the high pass filter must be higher than the fc of the low pass filter. " << endl;

	BPS BSfilter;
	BSfilter.pickType();
	BSfilter.checkBSFilter();
}

void BPS::pickType(void)
{
	char type;

	cout << endl << "Filter Components" << endl;
	cout << "1. RC Filter" << endl;
	cout << "2. RL Filter" << endl;

	type = select(string("Select HP filter type ==> "), string("12"));

	if (type == '1')
	{
		RC f1;
		HP = f1;
	}
	else
	{
		RL f1;
		HP = f1;
	}

	type = select(string("Select LP filter type ==> "), string("12"));

	if (type == '1')
	{
		RC f1;
		LP = f1;
	}
	else
	{
		RL f1;
		LP = f1;
	}
}

void BPS::checkBPFilter(void)
{
	if (LP > HP)
	{
		fH = LP.fc;
		fL = HP.fc;
		cout << endl << "For this band-pass filter:" << endl;
		cout << "fH: " << fL << " Hz." << endl;
		cout << "fL: " << fH << " Hz.";
	}
	else if (LP == HP)
	{
		cout << endl << "The values chosen are not a valid combination for a band-pass filter." << endl;
		cout << endl << "Both cutoff frequencies are the same." << endl;
	}
	else
	{
		cout << endl << "The values chosen are not a valid combination for a band-pass filter." << endl;
		cout << endl << "The fc of the low pass filter is lower than the fc of the high pass filter. " << endl;
	}
}

void BPS::checkBSFilter(void)
{
	if (HP > LP)
	{
		fH = HP.fc;
		fL = LP.fc;
		cout << endl << "For this band-stop filter:" << endl;
		cout << "fH: " << fL << " Hz." << endl;
		cout << "fL: " << fH << " Hz.";
	}
	else if (HP == LP)
	{
		cout << endl << "The values chosen are not a valid combination for a band-pass filter." << endl;
		cout << endl << "Both cutoff frequencies are the same." << endl;
	}
	else
	{
		cout << "The values chosen are not a valid combination for a band-stop filter." << endl;
		cout << "The fc of the high pass filter is lower than the fc of the low pass filter. "  << endl;
	}
}

void Filter::pickTypeof(string e, int num)
{
	char type;
	float freq = 1;

	cout << endl << endl << "1. RC " << e << " Filter" << endl;
	cout << "2. RL " << e << " Filter" << endl;

	type = select(string("Select filter type ==> "), string("12"));
	freq = getValue("Enter frequency to analyze in Hz ==>  ");

	if (type == '1')
	{
		RC filter;

		if (num == 1)
		{
			filter.calcHPAv(freq);
		}
		else
		{
			filter.calcLPAv(freq);
		}

		cout << endl << filter;
		cout << endl << "At " << freq << " Hz, gain = " << filter.gain ;
	}
	else if (type == '2')
	{
		RL filter;

		if (num == 1)
		{
			filter.calcHPAv(freq);
		}
		else
		{
			filter.calcLPAv(freq);
		}

		cout << endl << filter;
		cout << endl << "At " << freq << " Hz,  gain = " << filter.gain;
	}
}

int main(void)
{
	for (;;)
	{
		cout << setiosflags(ios::fixed) << setprecision(2);
		Circuit::Menu();
	}

	return 0;
}

