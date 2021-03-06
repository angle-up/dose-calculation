/**
 * \file
 * \brief Function implementation for reading DICOM images
 */

#include "dicom_reader.h"
#include "float3_affine_transform.cuh"
#include "cuda_runtime.h"

#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"

Float3AffineTransform itk_reader(const std::string imagePath, std::vector<float>& imageData, unsigned int& N, uint3& dim, int& t)
{
    typedef signed short PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::ImageSeriesReader<ImageType> ReaderType;
    typedef itk::MetaDataDictionary DictionaryType;
    typedef itk::GDCMImageIO ImageIOType;
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;

    Matrix3x3 imSpacing(0.);
    Matrix3x3 imDir(0.);
    float3 imOrigin;

    ReaderType::Pointer reader = ReaderType::New();
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO(dicomIO);
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails(true);
    nameGenerator->AddSeriesRestriction("0008|0021");
    nameGenerator->SetDirectory(imagePath);

    try {
        std::cout << "The directory " << imagePath << " contains the following DICOM Series:\n";

        typedef std::vector<std::string> SeriesIdContainer;
        const SeriesIdContainer &seriesUID = nameGenerator->GetSeriesUIDs();

        SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
        SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
        while( seriesItr != seriesEnd ) {
            std::cout << seriesItr->c_str() << std::endl << std::endl;
            ++seriesItr;
        }

        std::string seriesIdentifier;
        seriesIdentifier = seriesUID.begin()->c_str();

        std::cout << "Please wait while reading series:\n";
        std::cout << seriesIdentifier << std::endl;
        t = clock();

        typedef std::vector<std::string> FileNamesContainer;
        FileNamesContainer fileNames;

        fileNames = nameGenerator->GetFileNames(seriesIdentifier);

        reader->SetFileNames(fileNames);

        try {
            reader->Update();
        }
        catch (itk::ExceptionObject &ex) {
            std::cout << ex << std::endl;
            std::cin.get();
            throw std::runtime_error("Abort due to ITK exception");
        }
        //const DictionaryType &dictionary = dicomIO->GetMetaDataDictionary();
        //DictionaryType::ConstIterator itr = dictionary.Begin();
        //DictionaryType::ConstIterator end = dictionary.End();

        //while( itr != end ) {
        //  std::cout << itr->first << " " << itr->second << '\n';
        //  itr++;
        //}

        t = clock()-t;
        std::cout << "Done!\n\nRead image: " << (float)t/CLOCKS_PER_SEC << " seconds\n\n";

        ImageType::Pointer outputImagePtr = reader->GetOutput();
        itk::ImageRegionIterator<ImageType> it(outputImagePtr, outputImagePtr->GetLargestPossibleRegion());

        ImageType::SizeType imageSize = outputImagePtr->GetLargestPossibleRegion().GetSize();
        dim = make_uint3(uint(imageSize[0]), uint(imageSize[1]), uint(imageSize[2]));
        N = dim.x*dim.y*dim.z;
        imageData.resize(N);

        t = clock();

        it.GoToBegin();
        unsigned int idx = 0;
        while(!it.IsAtEnd())
        {
            imageData[idx] = float(it.Get() + 1000); ///\todo Image should be in HU+1000, remember? ///rescale intercept??
            ++idx;
            ++it;
        }

        //PixelType *pixelPtr = outputImagePtr->GetBufferPointer();
        //for(int i=0; i<N; i++) {
        //  imageData[i] = std::max<float>(float(pixelPtr[i]+1000)*0.001f,0.0f);
        //  imageData[i] = std::max<float>(float(pixelPtr[i]), -1000.0f);
        //}
        t = clock()-t;
        std::cout << "Convert image to float (CPU): " << (float)t/CLOCKS_PER_SEC << " seconds.\n\n";

        itk::Matrix<double,3,3> itkImDir = outputImagePtr->GetDirection();
        itk::Vector<double,3> itkImSpacing = outputImagePtr->GetSpacing();
        itk::Point<double,3> itkImOrigin = outputImagePtr->GetOrigin();
        imDir = Matrix3x3(make_float3(itkImDir[0][0],itkImDir[0][1],itkImDir[0][2]), make_float3(itkImDir[1][0],itkImDir[1][1],itkImDir[1][2]), make_float3(itkImDir[2][0],itkImDir[2][1],itkImDir[2][2]));
        imSpacing = Matrix3x3(make_float3(itkImSpacing.GetDataPointer()[0],itkImSpacing.GetDataPointer()[1],itkImSpacing.GetDataPointer()[2]));
        imOrigin = make_float3(itkImOrigin.GetDataPointer()[0],itkImOrigin.GetDataPointer()[1],itkImOrigin.GetDataPointer()[2]);
    }
    catch (itk::ExceptionObject &ex) {
      std::cout << ex << std::endl;
      throw std::runtime_error("Abort due to ITK exception");
    }
    return Float3AffineTransform(imDir*imSpacing, imOrigin);
}
