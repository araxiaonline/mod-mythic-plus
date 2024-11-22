package main

import (
	"fmt"
	"log"
	"math"

	"gonum.org/v1/gonum/mat"
)

// Data structure to hold the input data
type Data struct {
	Level             float64
	Class             int
	BaseHP0           float64
	BaseHP1           float64
	BaseHP2           float64
	BaseMana          float64
	BaseArmor         float64
	AttackPower       float64
	RangedAttackPower float64
	DamageBase        float64
	DamageExp1        float64
	DamageExp2        float64
	Comment           *string
}

func main() {
	// Input data (levels 68 to 80 for each class)
	data := []Data{
		{68, 1, 3834, 6542, 6986, 0, 6126, 292, 41, 39.2381, 94.4934, 104.527, nil},
		{68, 2, 3067, 5233, 6986, 2991, 6116, 276, 31, 36.3244, 87.2677, 104.527, nil},
		{68, 4, 3834, 6542, 6986, 0, 5527, 292, 41, 39.2381, 94.4934, 104.527, nil},
		{68, 8, 2684, 4580, 5588, 6882, 4928, 130, 27, 33.3048, 80.1061, 96.6868, nil},
		{69, 1, 3942, 6761, 7984, 0, 6423, 298, 43, 39.9047, 99.5328, 114.153, nil},
		{69, 2, 3153, 5409, 7984, 3080, 6412, 282, 32, 36.974, 91.8916, 114.153, nil},
		{69, 4, 3942, 6761, 7984, 0, 5795, 298, 43, 39.9047, 99.5328, 114.153, nil},
		{69, 8, 2759, 4733, 6387, 7031, 5167, 133, 28, 33.8695, 84.2722, 105.591, nil},
		{70, 1, 4050, 6986, 8982, 0, 6719, 304, 44, 40.5714, 104.527, 123.779, nil},
		{70, 2, 3240, 5589, 8982, 3155, 6708, 286, 33, 37.6361, 96.7364, 123.779, nil},
		{70, 4, 4050, 6986, 8982, 0, 6062, 304, 44, 40.5714, 104.527, 123.779, nil},
		{70, 8, 2835, 4890, 7185, 7196, 5404, 135, 28, 34.4369, 88.3402, 114.496, nil},
		{71, 1, 4163, 7181, 9291, 0, 7018, 308, 48, 41.2381, 106.357, 127.382, nil},
		{71, 2, 3330, 5744, 9291, 3231, 7007, 290, 37, 38.2899, 98.3977, 127.383, nil},
		{71, 4, 4163, 7181, 9291, 0, 6332, 308, 48, 41.2381, 106.357, 127.382, nil},
		{71, 8, 2914, 5027, 7432, 7332, 5645, 137, 31, 35.0025, 92.4034, 117.829, nil},
		{72, 1, 4278, 7380, 9610, 0, 7318, 314, 53, 41.9047, 108.071, 131.091, nil},
		{72, 2, 3422, 5903, 9610, 3309, 7305, 296, 40, 38.9492, 99.8571, 131.092, nil},
		{72, 4, 4278, 7380, 9610, 0, 6602, 314, 53, 41.9047, 108.071, 131.091, nil},
		{72, 8, 2995, 5166, 7688, 7500, 5886, 140, 34, 35.5693, 96.5068, 121.259, nil},
		{73, 1, 4399, 7588, 9940, 0, 7618, 320, 58, 42.5714, 118.643, 134.908, nil},
		{73, 2, 3519, 6070, 9940, 3387, 7604, 302, 44, 39.6048, 101.451, 134.908, nil},
		{73, 4, 4399, 7580, 9940, 0, 6872, 320, 58, 42.5714, 118.643, 134.908, nil},
		{73, 8, 3098, 5311, 7952, 7654, 6126, 143, 37, 36.1353, 100.617, 124.79, nil},
		{74, 1, 4524, 7804, 10282, 0, 7918, 354, 63, 43.2381, 120.434, 138.836, nil},
		{74, 2, 3619, 6243, 10282, 3466, 7903, 334, 48, 40.2629, 102.955, 138.836, nil},
		{74, 4, 4524, 1, 10282, 0, 7143, 354, 63, 43.2381, 120.434, 138.836, nil},
		{74, 8, 3186, 1, 8225, 7809, 6368, 158, 41, 36.7018, 104.723, 128.423, nil},
		{75, 1, 4652, 8025, 10635, 0, 8219, 392, 68, 43.9047, 122.226, 142.878, nil},
		{75, 2, 3722, 6420, 10635, 3561, 8204, 370, 53, 40.9193, 104.52, 142.878, nil},
		{75, 4, 4652, 1, 10635, 0, 7415, 392, 68, 43.9047, 122.226, 142.878, nil},
		{75, 8, 3256, 5617, 8508, 7981, 6610, 175, 45, 37.268, 108.832, 132.162, nil},
		{76, 1, 4781, 8247, 11001, 0, 8520, 432, 74, 44.5713, 124.018, 147.038, nil},
		{76, 2, 3825, 6602, 11001, 3643, 8503, 408, 57, 41.5757, 106.085, 147.038, nil},
		{76, 4, 4781, 1, 11001, 0, 7686, 432, 74, 44.5713, 124.018, 147.038, nil},
		{76, 8, 3367, 1, 8800, 8139, 6851, 193, 49, 37.8342, 112.941, 136.01, nil},
		{77, 1, 4916, 8480, 11379, 0, 8822, 478, 81, 45.2379, 125.81, 151.319, nil},
		{77, 2, 3933, 6784, 11379, 3725, 8803, 452, 62, 42.2321, 107.65, 151.319, nil},
		{77, 4, 4916, 1, 11379, 0, 7958, 478, 81, 45.2379, 125.81, 151.319, nil},
		{77, 8, 3462, 1, 9103, 8313, 7094, 214, 54, 38.4004, 117.05, 139.97, nil},
		{78, 1, 5052, 8715, 11770, 0, 9124, 528, 88, 45.9045, 127.602, 155.724, nil},
		{78, 2, 4042, 6972, 11770, 3809, 9104, 500, 68, 42.8885, 109.215, 155.724, nil},
		{78, 4, 5052, 1, 11770, 0, 8230, 528, 88, 45.9045, 127.602, 155.724, nil},
		{78, 8, 3558, 1, 9416, 8459, 7335, 236, 58, 38.9666, 121.159, 144.045, nil},
		{79, 1, 5194, 8960, 12175, 0, 9426, 582, 95, 46.5711, 129.394, 160.258, nil},
		{79, 2, 4155, 7167, 12175, 3893, 9405, 550, 74, 43.5449, 110.78, 160.258, nil},
		{79, 4, 5194, 1, 12175, 0, 8503, 582, 95, 46.5711, 129.394, 160.258, nil},
		{79, 8, 3658, 1, 9740, 8636, 7579, 260, 64, 39.5328, 125.268, 148.239, nil},
		{80, 1, 5342, 9215, 12600, 0, 9729, 642, 103, 47.2377, 131.186, 164.924, nil},
		{80, 2, 4274, 7373, 12600, 3994, 9706, 608, 80, 44.2013, 112.345, 164.924, nil},
		{80, 4, 5342, 1, 12600, 0, 8776, 642, 103, 47.2377, 131.186, 164.924, nil},
		{80, 8, 3739, 1, 10080, 8814, 7822, 287, 69, 40.099, 129.377, 152.555, nil},
	}

	// Classes to process
	classes := []int{1, 2, 4, 8}

	// Columns to process
	columns := []string{"BaseHP0", "BaseHP1", "BaseHP2", "BaseMana", "BaseArmor", "AttackPower", "RangedAttackPower", "DamageBase", "DamageExp1", "DamageExp2"}

	// Fit polynomials for each class and each attribute
	classCoefficients := make(map[int]map[string][]float64)

	for _, class := range classes {
		classData := filterDataByClass(data, class)

		// Store coefficients for each column for this class
		coefficients := make(map[string][]float64)

		for _, column := range columns {
			levels := make([]float64, 0)
			values := make([]float64, 0)

			for _, d := range classData {
				var value float64
				switch column {
				case "BaseHP0":
					value = d.BaseHP0
				case "BaseHP1":
					value = d.BaseHP1
				case "BaseHP2":
					value = d.BaseHP2
				case "BaseMana":
					value = d.BaseMana
				case "BaseArmor":
					value = d.BaseArmor
				case "AttackPower":
					value = d.AttackPower
				case "RangedAttackPower":
					value = d.RangedAttackPower
				case "DamageBase":
					value = d.DamageBase
				case "DamageExp1":
					value = d.DamageExp1
				case "DamageExp2":
					value = d.DamageExp2
				}

				if value > 1 {
					levels = append(levels, d.Level)
					values = append(values, value)
				}
			}

			if len(levels) == 0 || len(values) == 0 {
				continue // skip if no valid data
			}

			// Perform polynomial regression (degree 2)
			coeffs, err := polyFit(levels, values, 2)
			if err != nil {
				log.Fatalf("Failed to calculate polynomial fit for class %d, %s: %v", class, column, err)
			}

			coefficients[column] = coeffs
		}

		classCoefficients[class] = coefficients
	}

	// Generate SQL insert statements for levels 81 to 100
	fmt.Println("INSERT INTO mythicplus_classlevelstats (class, level, basehp0, basehp1, basehp2, basemana, basearmor, attackpower, rangedattackpower, damage_base, damage_exp1, damage_exp2) VALUES")

	for _, class := range classes {
		for level := 81.0; level <= 100; level++ {
			predictedValues := calculateScaledValues(level, classCoefficients[class])
			fmt.Printf("(%d, %.0f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.4f, %.4f, %.4f),\n",
				class, level,
				predictedValues["BaseHP0"], predictedValues["BaseHP1"], predictedValues["BaseHP2"], predictedValues["BaseMana"],
				predictedValues["BaseArmor"], predictedValues["AttackPower"], predictedValues["RangedAttackPower"],
				predictedValues["DamageBase"], predictedValues["DamageExp1"], predictedValues["DamageExp2"])
		}
	}
}

// filterDataByClass filters the dataset by class
func filterDataByClass(data []Data, class int) []Data {
	var filtered []Data
	for _, d := range data {
		if d.Class == class {
			filtered = append(filtered, d)
		}
	}
	return filtered
}

// polyFit performs a polynomial fit of the given degree on the data.
func polyFit(x, y []float64, degree int) ([]float64, error) {
	// Create the Vandermonde matrix
	n := len(x)
	vander := mat.NewDense(n, degree+1, nil)
	for i := range x {
		for j := 0; j <= degree; j++ {
			vander.Set(i, j, math.Pow(x[i], float64(j)))
		}
	}

	// Create a vector for the output values
	yVec := mat.NewVecDense(len(y), y)

	// Solve the least squares problem
	var coeffs mat.VecDense
	err := coeffs.SolveVec(vander, yVec)
	if err != nil {
		return nil, err
	}

	return coeffs.RawVector().Data, nil
}

// calculateScaledValues takes a level and coefficient map to return scaled values
func calculateScaledValues(level float64, coeffs map[string][]float64) map[string]float64 {
	values := make(map[string]float64)
	for column, c := range coeffs {
		// Use the polynomial formula: ax^2 + bx + c
		values[column] = c[2]*math.Pow(level, 2) + c[1]*level + c[0]
	}
	return values
}
