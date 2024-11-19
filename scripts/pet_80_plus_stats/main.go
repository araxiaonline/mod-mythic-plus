package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math"
	"os"
)

type Creature struct {
	CreatureEntry int `json:"creature_entry"`
	Level         int `json:"level"`
	HP            int `json:"hp"`
	Mana          int `json:"mana"`
	Armor         int `json:"armor"`
	Str           int `json:"str"`
	Agi           int `json:"agi"`
	Sta           int `json:"sta"`
	Int           int `json:"inte"`
	Spi           int `json:"spi"`
	MinDmg        int `json:"min_dmg"`
	MaxDmg        int `json:"max_dmg"`
}

// RegressionResult stores the slope and intercept of a linear regression
type RegressionResult struct {
	Slope     float64
	Intercept float64
}

// LinearRegression performs a simple linear regression on the given data points
func LinearRegression(levels []float64, stats []float64) RegressionResult {
	n := len(levels)
	if n != len(stats) {
		panic("levels and stats arrays must have the same length")
	}

	// Calculate averages
	var sumX, sumY, sumXY, sumXX float64
	for i := 0; i < n; i++ {
		sumX += levels[i]
		sumY += stats[i]
		sumXY += levels[i] * stats[i]
		sumXX += levels[i] * levels[i]
	}

	avgX := sumX / float64(n)
	avgY := sumY / float64(n)

	// Calculate slope and intercept
	slope := (sumXY - float64(n)*avgX*avgY) / (sumXX - float64(n)*avgX*avgX)
	intercept := avgY - slope*avgX

	return RegressionResult{
		Slope:     slope,
		Intercept: intercept,
	}
}

// PredictStat returns the predicted stat value for a given level based on a linear regression model
func PredictStat(reg RegressionResult, level int) float64 {
	return reg.Intercept + reg.Slope*float64(level)
}

// ScaleCreatures generates stats for levels 81 to 87 based on linear regression and outputs SQL statements
func ScaleCreatures(creatures []Creature) {
	creatureMap := make(map[int][]Creature)

	// Group creatures by their creature_entry
	for _, creature := range creatures {
		creatureMap[creature.CreatureEntry] = append(creatureMap[creature.CreatureEntry], creature)
	}

	// Process each creature entry independently
	for creatureEntry, creatureList := range creatureMap {
		var levels []float64
		var hp, mana, armor, str, agi, sta, inte, spi, minDmg, maxDmg []float64

		// Collect data for linear regression from levels 1 to 80
		for _, creature := range creatureList {
			if creature.Level <= 80 {
				levels = append(levels, float64(creature.Level))
				hp = append(hp, float64(creature.HP))
				mana = append(mana, float64(creature.Mana))
				armor = append(armor, float64(creature.Armor))
				str = append(str, float64(creature.Str))
				agi = append(agi, float64(creature.Agi))
				sta = append(sta, float64(creature.Sta))
				inte = append(inte, float64(creature.Int))
				spi = append(spi, float64(creature.Spi))
				minDmg = append(minDmg, float64(creature.MinDmg))
				maxDmg = append(maxDmg, float64(creature.MaxDmg))
			}
		}

		// Perform linear regression on each stat
		hpReg := LinearRegression(levels, hp)
		manaReg := LinearRegression(levels, mana)
		armorReg := LinearRegression(levels, armor)
		strReg := LinearRegression(levels, str)
		agiReg := LinearRegression(levels, agi)
		staReg := LinearRegression(levels, sta)
		inteReg := LinearRegression(levels, inte)
		spiReg := LinearRegression(levels, spi)
		minDmgReg := LinearRegression(levels, minDmg)
		maxDmgReg := LinearRegression(levels, maxDmg)

		// Generate SQL insert statements for levels 81 to 87 for each creature_entry
		for level := 81; level <= 87; level++ {
			newCreature := Creature{
				CreatureEntry: creatureEntry, // Process per creature_entry
				Level:         level,
				HP:            int(math.Round(math.Max(PredictStat(hpReg, level), 1))), // Round and ensure positive
				Mana:          int(math.Round(math.Max(PredictStat(manaReg, level), 1))),
				Armor:         int(math.Round(math.Max(PredictStat(armorReg, level), 1))), // Armor scaling
				Str:           int(math.Round(math.Max(PredictStat(strReg, level), 1))),
				Agi:           int(math.Round(math.Max(PredictStat(agiReg, level), 1))),
				Sta:           int(math.Round(math.Max(PredictStat(staReg, level), 1))),
				Int:           int(math.Round(math.Max(PredictStat(inteReg, level), 1))),
				Spi:           int(math.Round(math.Max(PredictStat(spiReg, level), 1))),
				MinDmg:        int(math.Round(math.Max(PredictStat(minDmgReg, level), 1))),
				MaxDmg:        int(math.Round(math.Max(PredictStat(maxDmgReg, level), 1))),
			}

			// Output SQL INSERT statement for each creature_entry and level
			fmt.Printf("INSERT INTO pet_levelstats (creature_entry, level, hp, mana, armor, str, agi, sta, inte, spi, min_dmg, max_dmg) "+
				"VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);\n",
				newCreature.CreatureEntry, newCreature.Level, newCreature.HP, newCreature.Mana, newCreature.Armor,
				newCreature.Str, newCreature.Agi, newCreature.Sta, newCreature.Int, newCreature.Spi,
				newCreature.MinDmg, newCreature.MaxDmg)
		}
	}
}

func main() {
	// Read the JSON file
	file, err := os.Open("creatures.json")
	if err != nil {
		fmt.Println("Error reading file:", err)
		return
	}
	defer file.Close()

	byteValue, _ := ioutil.ReadAll(file)

	// Parse the JSON data
	var creatures []Creature
	json.Unmarshal(byteValue, &creatures)

	// Scale creatures and generate SQL for levels 81 to 87
	ScaleCreatures(creatures)
}
