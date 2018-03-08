#include "wcdCFacerecognize.h"

using namespace std;
using namespace cv;

wcdCFacerecognize::wcdCFacerecognize()
{

}

wcdCFacerecognize::wcdCFacerecognize(string path,string prototxt,string caffemodel,std::string seeta_detect_path,std::string dlib_path)
{
    m_sql.reset(new MyDB());
    head_path = path;
	m_name = LoadName();
	m_features = LoadFaceMatrix();

	ex.reset(new ExtractFeature(prototxt,caffemodel));
	model_path=seeta_detect_path;
	face_align=dlib_path;
	dlib::deserialize(dlib_path) >> sp;
}

wcdCFacerecognize::~wcdCFacerecognize()
{
   // delete ex;
   // delete m_sql;
}

int wcdCFacerecognize::m_AddVerifyTarget(string VTGUID, Mat iptFrame)
{
    if(iptFrame.empty())
    {
        cout<<"Mat is empty(func m_AddVerifyTarget)"<<endl;
        return -1;
    }

    Mat img=iptFrame;

    vector<Mat> faces=FaceDetect(img);

    if(faces.size()>1)
    {
       cout<<"Too much faces(func m_AddVerifyTarget)"<<endl;
       return -1;
    }
    else if(faces.size()==0)
        return -1;

    vector<float> feature;
    feature=ex->Extract(faces[0]);

    if(feature.size()==0)
    {
        cout<<"Can not extract feature(func m_AddVerifyTarget)"<<endl;
        return -1;
    }


    string name(VTGUID);
	vector<string>::iterator it = find(m_name.begin(), m_name.end(), name);
	if (it != m_name.end())
	{
		cout << "Name existed,please use others!(Func m_AddVerifyTarget)" << endl;
		return -1;
	}

	string saveStr=vector2string(feature);
	string sql="insert into t_features value(0,'"+saveStr+"')";
	m_sql->sql_insert(sql);
	sql="insert into t_name value(0,'"+name+"')";
	m_sql->sql_insert(sql);

	cout << "ADD No." << name << " to database successfully!" << endl;
	return 0;

}

int wcdCFacerecognize::m_DelVerifyTarget(string VTGUID)
{
    if (VTGUID.size()==0)
	{
		cout << "VTGUID is empty(Func FR_DelVerifyTarget)" << endl;
		return -1;
	}
	string name(VTGUID);
	string sql="select id,name from t_name where name="+name;
	bool name_exist=m_sql->sql_isExisted(sql);
	assert(true==name_exist);

	sql="select id from t_name where name="+name;
	string m_id=m_sql->sql_get(sql);

	sql="delete from t_features where id="+m_id;
	m_sql->sql_delete(sql);
	sql="delete from t_name where id="+m_id;
	m_sql->sql_delete(sql);

	cout << "Delete No." << name << " from database successfully!" << endl;
	return 0;
}

int wcdCFacerecognize::m_ClearVerifyTarget()
{
    int ret = 0;
	for (int i = 0; i < m_name.size(); i++)
	{
		ret = m_DelVerifyTarget(m_name[i]);
		if (ret == -1)
			return -1;
	}

	cout << "Delete all successfully!" << endl;
	return 0;
}

vector<faceData> wcdCFacerecognize::m_FaceListVerify(Mat iptFrame, string& VTGUID)
{
    vector<Mat> detect_face = FaceDetect(iptFrame);

	if (detect_face.empty())
	{
		cout << "image has no face1(Func m_Multi_FaceListVerify)" << endl;
		assert(false);
	}

	vector<float> feature;
	vector<faceData> temp_data;

	for (int j = 0; j < detect_face.size(); j++)
	{
		feature = ex->Extract(detect_face[j]);

		if (feature.size() == 0)
		{
			cout << "Image "<<j<<" can't extract feature!(Func m_Multi_FaceListVerify)" << endl;
			continue;
		}
		for (int i = 1; i < m_features.size() + 1; i++)
		{
			float rate = cosine(feature, m_features[i - 1]);

			faceData temp(stoi(m_name[i-1]),rate);
			temp_data.push_back(temp);

		}

	}
	sort(temp_data.begin(),temp_data.end());
	//for(int i=0;i<temp_data.size();i++)
    //{
    //    cout<<temp_data[i].rate<<endl;
    //}
	vector<faceData> res(temp_data.end()-3,temp_data.end());
	return res;
}

void wcdCFacerecognize::Caffe_Init()
{
    ex->Caffe_Predefine();
}



vector<Mat> wcdCFacerecognize::FaceDetect(Mat iptFrame)
{
    assert(iptFrame.empty()==0);

    Mat img=iptFrame,img_gray;

    seeta::FaceDetection detector(model_path.c_str());
    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    if (img.channels() != 1)
        cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = img;

    seeta::ImageData img_data;
    img_data.data = img_gray.data;
    img_data.width = img_gray.cols;
    img_data.height = img_gray.rows;
    img_data.num_channels = 1;

    std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);

    //#ifdef USE_OPENMP
    //    cout << "OpenMP is used." << endl;
    //#else
    //    cout << "OpenMP is not used. " << endl;
    //#endif

    //#ifdef USE_SSE
    //    cout << "SSE is used." << endl;
    //#else
    //    cout << "SSE is not used." << endl;
    //#endif

    //cout << "Image size (wxh): " << img_data.width << "x"
    //<< img_data.height << endl;

    vector<Mat> res;
    Mat mat;
    cv::Rect face_rect;
    int32_t num_face = static_cast<int32_t>(faces.size());
    //cout<<num_face<<endl;

    for (int32_t i = 0; i < num_face; i++) {
        face_rect.x = max(faces[i].bbox.x,0);
        face_rect.y = max(faces[i].bbox.y,0);
        face_rect.width = min(faces[i].bbox.width,img.cols-faces[i].bbox.width);
        face_rect.height = min(faces[i].bbox.height,img.rows-faces[i].bbox.height);

        cv::Rect opencvRect=face_rect;
        dlib::rectangle dlibRect((long)opencvRect.tl().x, (long)opencvRect.tl().y, (long)opencvRect.br().x - 1, (long)opencvRect.br().y - 1);
		dlib::full_object_detection shape = sp(dlib::cv_image<uchar>(img_gray), dlibRect);
		std::vector<dlib::full_object_detection> shapes;
		shapes.push_back(shape);
		dlib::array<dlib::array2d<dlib::rgb_pixel>>  face_chips;
		extract_image_chips(dlib::cv_image<uchar>(img_gray), get_face_chip_details(shapes), face_chips);
		Mat pic = dlib::toMat(face_chips[0]);
        resize(pic, pic, Size(224, 224));
        res.push_back(pic);
        //res.push_back(FaceProcessing(pic));
    }

    return res;

}

Mat wcdCFacerecognize::Vector2dToMat(vector<float> feature)
{
    Mat T(1, 4096, CV_32F);
	for (int i = 0; i < feature.size(); i++)
	{
		T.at<float>(0, i) = feature[i];
	}
	return T;
}

void wcdCFacerecognize::SaveMat(Mat &saveFeature, const string& filename)
{
    FileStorage fs(filename, FileStorage::WRITE);
	if (!fs.isOpened())
	{
		cout << "Can't open file,please input right path!(Func SaveMat)" << endl;
		return;
	}

	fs << "FaceMatrix" << saveFeature;
	fs.release();
}

void wcdCFacerecognize::SaveName(const string& name, const string& filename)
{

    ofstream out_(filename.c_str(), ofstream::app);
	if (!out_)
	{
		cout << "Can't open file,please input right path!(Func SaveName)" << endl;
		return;
	}
	out_ << name << endl;
}

float wcdCFacerecognize::cosine(const vector<float>& v1, const vector<float>& v2)
{
    assert(v1.size() == v2.size());
	float f = 0.0;
	if ((module(v1) * module(v2)) == 0)
		f = 0;
	else
		f = dotProduct(v1, v2) / (module(v1) * module(v2));
	return f;
}


float wcdCFacerecognize::dotProduct(const vector<float>& v1, const vector<float>& v2)
{
    assert(v1.size() == v2.size());
	float ret = 0.0;
	for (vector<float>::size_type i = 0; i != v1.size(); ++i)
	{
		if (v1[i] > 1000 || v2[i] > 1000 || v1[i] < (-1000) || v2[i] < (-1000))
			continue;
		else if (!(isnan(v1[i]) || isnan(v2[i])))
			ret += v1[i] * v2[i];
	}
	return ret;
}

float wcdCFacerecognize::module(const vector<float>& v)
{
    float ret = 0.0;
	for (vector<float>::size_type i = 0; i != v.size(); ++i)
	{
		if (v[i] > 1000 || v[i]<(-1000))
			continue;
		else if (!isnan(v[i]))
			ret += v[i] * v[i];
	}
	return sqrt(ret);
}

vector<string> wcdCFacerecognize::LoadName()
{
    vector<string> name;
	string sql="select name from t_name";
	name=m_sql->sql_get_all(sql);
	return name;
}

vector<float> wcdCFacerecognize::Mat2vector(Mat &FaceMatrix_mat)
{
    vector<float> v;
	for (int i = 0; i < FaceMatrix_mat.rows; ++i)
		for (int j = 0; j < FaceMatrix_mat.cols; ++j)
			v.push_back(FaceMatrix_mat.at<float>(i, j));
	return v;
}

vector<vector<float> > wcdCFacerecognize::LoadFaceMatrix()
{
    vector<vector<float> > features;
	vector<string> temp;
	string sql="select feature from t_features";
	temp=m_sql->sql_get_all(sql);
	vector<float> temp_v;
	for(int i=0;i<temp.size();i++)
    {
        temp_v=string2vector(temp[i]);
        features.push_back(temp_v);
    }
	return features;
}

string wcdCFacerecognize::vector2string(vector<float> v)
{
    assert(0!=v.size());
    assert(FEATURE_NUM==v.size());
    string res;
    for(int i=0;i<v.size();i++)
    {
        if(0==i)
            res+=to_string(v[i]);
        else
            res=res+" "+to_string(v[i]);

    }
    return res;
}

vector<float> wcdCFacerecognize::string2vector(const string& s)
{
    istringstream is(s);
    vector<float> res;
    float temp;
    while(is)
    {
        is>>temp;
        res.push_back(temp);
    }
    res.pop_back();
    assert(FEATURE_NUM==res.size());
    return res;
}

double wcdCFacerecognize::MatMaxMin(Mat im, string flag )
{
	double value = im.ptr<float>(0)[0];
	if (flag == "MAX")
	{
		for (int i = 0; i<im.rows; i++)
			for (int j = 0; j<im.cols; j++)
				if (im.ptr<float>(i)[j]>value)
					value = im.ptr<float>(i)[j];
		return value;
	}
	else if (flag == "MIN")
	{
		for (int i = 0; i<im.rows; i++)
			for (int j = 0; j<im.cols; j++)
				if (im.ptr<float>(i)[j]<value)
					value = im.ptr<float>(i)[j];
		return value;
	}
	return -1;
}

Mat wcdCFacerecognize::gaussianfilter(Mat img, double sigma0, double sigma1, double shift1 = 0, double shift2 = 0)
{
	int i, j;
	sigma0 = (float)sigma0;
	sigma1 = (float)sigma1;
	shift1 = (float)shift1;
	shift2 = (float)shift2;
	Mat img2 = img;
	Mat img3 = img;
	Mat  imgResult;

	int rowLength = (int)(floor(3.0*sigma0 + 0.5 - shift1) - ceil(-3.0*sigma0 - 0.5 - shift1) + 1);
	int rowBegin = (int)ceil(-3.0*sigma0 - 0.5 - shift1);
	float rowArray[30], Gx[30];
	for (i = 0; i < rowLength; i++)
	{
		rowArray[i] = rowBegin + i;
	}
	gauss(rowArray, Gx, rowLength, sigma0);
	Mat kx = Mat(1, rowLength, CV_32F);
	float *pData1 = kx.ptr<float>(0);
	for (i = 0; i < rowLength; i++)
	{
		pData1[i] = Gx[i];
	}

	int colLength = (int)(floor(3.0*sigma1 + 0.5 - shift2) - ceil(-3.0*sigma1 - 0.5 - shift2) + 1);
	int colBegin = (int)ceil(-3.0*sigma1 - 0.5 - shift2);
	float colArray[30], Gy[30];
	for (i = 0; i<colLength; i++)
	{
		colArray[i] = colBegin + i;
	}
	gauss(colArray, Gy, colLength, sigma1);
	Mat ky = Mat(colLength, 1, CV_32F);
	float *pData2;
	for (i = 0; i < colLength; i++)
	{
		pData2 = ky.ptr<float>(i);
		pData2[0] = Gy[i];
	}
	filter2D(img, img2, img.depth(), kx, Point(-1, -1));
	filter2D(img2, imgResult, img2.depth(), ky, Point(-1, -1));
	return imgResult;

}

int wcdCFacerecognize::gauss(float x[], float y[], int length, float sigma)
{
	int i;
	float sum = 0.0;
	for (i = 0; i<length; i++)
	{
		x[i] = exp(-pow(x[i], 2) / (2 * pow(sigma, 2)));
		sum += x[i];
	}
	for (i = 0; i<length; i++)
	{
		y[i] = x[i] / sum;
	}
	return 1;
}


Mat wcdCFacerecognize::FaceProcessing(const Mat &img_, double gamma, double sigma0, double sigma1, double mask, double do_norm)
{
	Mat img;
	img_.convertTo(img, CV_32F);
	Mat imT1, imT2;
	int rows = img.rows;
	int cols = img.cols;
	Mat im = img;
	int b = floor(3 * abs(sigma1));
	Mat imtemp(Size(cols + 2 * b, rows + 2 * b), CV_32F, Scalar(0));
	Mat imtemp2(Size(cols, rows), CV_32F, Scalar(0));
	float s = 0.0;
	//Gamma correct input image to increase local contrast in shadowed regions.
	if (gamma == 0)
	{
		double impixeltemp = 0;
		double Max = MatMaxMin(im, "MAX");//等价于max(1,max(max(im)))
		for (int i = 0; i<rows; i++)
			for (int j = 0; j<cols; j++)
			{
				impixeltemp = log(im.ptr<float>(i)[j] + Max / 256);
				im.ptr<float>(i)[j] = impixeltemp;
			}
	}
	else
	{
		for (int i = 0; i<rows; i++)
			for (int j = 0; j<cols; j++)
				im.ptr<float>(i)[j] = pow(im.ptr<float>(i)[j], gamma);
	}
	float *pData1;

	if (sigma1)
	{
		double border = 1;
		if (border) //add extend-as-constant image border to reduce

		{
			for (int i = 0; i<rows + 2 * b - 1; i++)
			{
				pData1 = imtemp.ptr<float>(i);
				for (int j = 0; j<cols + 2 * b - 1; j++){
					//中间
					if (i >= b&&i<im.rows + b&&j >= b&&j<im.cols + b)
						pData1[j] = im.ptr<float>(i - b)[j - b];
					//左上
					else if (i<b&&j<b)
						pData1[j] = im.ptr<float>(0)[0];
					//右上
					else if (i<b&&j >= im.cols + b&&j<cols + 2 * b)
						pData1[j] = im.ptr<float>(0)[cols - 1];
					//左下
					else if (i >= im.rows + b&&i<rows + 2 * b&&j<b)
						pData1[j] = im.ptr<float>(rows - 1)[0];
					//右下
					else if (i >= im.rows + b&&j >= im.cols + b)
						pData1[j] = im.ptr<float>(im.rows - 1)[im.cols - 1];
					//上方
					else if (i<b&&j >= b&&j<im.cols + b)
						pData1[j] = im.ptr<float>(0)[j - b];
					//下方
					else if (i >= im.rows + b&&j >= b&&j<im.cols + b)
						pData1[j] = im.ptr<float>(im.rows - 1)[j - b];
					//左方
					else if (j<b&&i >= b&&i<im.rows + b)
						pData1[j] = im.ptr<float>(i - b)[0];
					//右方
					else if (j >= im.cols + b&&i >= b&&i<im.rows + b)
						pData1[j] = im.ptr<float>(i - b)[im.cols - 1];/**/
				}
			}
		}

		else
		{
			if (sigma0>0)
			{
				imT1 = gaussianfilter(imtemp, sigma0, sigma0);
				imT2 = gaussianfilter(imtemp, -sigma1, -sigma1);
				imtemp = imT1 - imT2;
				//imtemp=gaussianfilter(imtemp,sigma0,sigma0)-gaussianfilter(imtemp,-sigma1,-sigma1);
			}
			else
				imtemp = imtemp - gaussianfilter(imtemp, -sigma1, -sigma1);
		}

		if (border)
		{
			//再取回中间部分
			for (int i = 0; i<rows; i++)
			{
				pData1 = im.ptr<float>(i);
				for (int j = 0; j<cols; j++)
					pData1[j] = imtemp.ptr<float>(i + b)[j + b];
			}
		}
		//  test=im.ptr<float>(19)[19];
	}

	/*
	% Global contrast normalization. Normalizes the spread of output
	% values. The mean is near 0 so we don't bother to subtract
	% it. We use a trimmed robust scatter measure for resistance to
	% outliers such as specularities and image borders that have
	% different values from the main image.  Usually trim is about
	% 10.
	*/

	if (do_norm)
	{
		double a = 0.1;
		double trim = abs(do_norm);

		//im = im./mean(mean(abs(im).^a))^(1/a);
		imtemp2 = abs(im);


		for (int i = 0; i<rows; i++)
		{
			pData1 = imtemp2.ptr<float>(i);//imtemp2为零矩阵
			for (int j = 0; j<cols; j++)
				pData1[j] = pow(imtemp2.ptr<float>(i)[j], a);
		}


		s = 0.0;
		for (int i = 0; i<rows; i++)
		{
			pData1 = imtemp2.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				s += imtemp2.ptr<float>(i)[j];
		}
		s /= (im.rows*im.cols);
		double temp = pow(s, 1 / a);
		for (int i = 0; i<rows; i++)
		{
			pData1 = im.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				pData1[j] = pData1[j] / temp;
		}


		imtemp2 = abs(im);
		for (int i = 0; i<rows; i++)
		{
			pData1 = imtemp2.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				if (pData1[j]>trim)
					pData1[j] = trim;//min(trim,abs(im))
		}

		for (int i = 0; i<rows; i++)
		{
			pData1 = imtemp2.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				pData1[j] = pow(pData1[j], a);
		}

		s = 0.0;
		for (int i = 0; i<rows; i++)
		{
			pData1 = imtemp2.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				s += pData1[j];
		}
		s /= (im.rows*im.cols);
		temp = pow(s, 1 / a);//
		for (int i = 0; i<rows; i++)
		{
			pData1 = im.ptr<float>(i);
			for (int j = 0; j<cols; j++)
				pData1[j] = pData1[j] / temp;
		}

		if (do_norm>0)
		{//im = trim*tanh(im/trim);
			for (int i = 0; i<rows; i++)
			{
				pData1 = im.ptr<float>(i);
				for (int j = 0; j<cols; j++)
					pData1[j] = trim*tanh(pData1[j] / trim);
			}
		}

	}

	double Min;
	Min = MatMaxMin(im, "MIN");
	for (int i = 0; i<rows; i++)
	{
		pData1 = im.ptr<float>(i);
		for (int j = 0; j<cols; j++)
			pData1[j] += Min;
	}
	//im.convertTo(im, CV_32F, 1.0/255.0);

	normalize(im, im, 0, 255, NORM_MINMAX);
	//normalize(im,im,0,255,NORM_MINMAX);
	/*  for(int i=0;i<rows;i++)
	{
	pData1=im.ptr<float>(i);
	for(int j=0;j<cols;j++)
	pData1[j]*=255;
	}*/
	im.convertTo(im, CV_8UC1);
	return im;
}











