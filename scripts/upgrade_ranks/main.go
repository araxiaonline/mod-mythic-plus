package main

import (
	"fmt"
	"math"
	"os"
)

// Stats enum
const (
	STAT_INTELLECT = iota
	STAT_SPIRIT
	STAT_STRENGTH
	STAT_AGILITY
	STAT_STAMINA
)

const (
	RESIST_ARCANE = iota
	RESIST_FIRE
	RESIST_NATURE
	RESIST_FROST
	RESIST_SHADOW
)

// Return the itemEntry for a state item and based on quality (3 - rare, 4 - epic)
func getStatItemEntry(statID int, quality int) int {
	switch statID {
	case STAT_INTELLECT:
		if quality == 3 {
			return 911005
		}
		return 911006
	case STAT_SPIRIT:
		if quality == 3 {
			return 911009
		}
		return 911010
	case STAT_STRENGTH:
		if quality == 3 {
			return 911003
		}
		return 911004
	case STAT_AGILITY:
		if quality == 3 {
			return 911007
		}
		return 911008
	case STAT_STAMINA:
		if quality == 3 {
			return 911011
		}
		return 911012

	default:
		return 0
	}
}

func getResistItemEntry(resistID int, quality int) int {
	switch resistID {
	case RESIST_FROST:
		if quality == 3 {
			return 911015
		}
		return 911016
	case RESIST_FIRE:
		if quality == 3 {
			return 911017
		}
		return 911018
	case RESIST_NATURE:
		if quality == 3 {
			return 911023
		}
		return 911024
	case RESIST_SHADOW:
		if quality == 3 {
			return 911021
		}
		return 911022
	case RESIST_ARCANE:
		if quality == 3 {
			return 911019
		}
		return 911020

	default:
		return 0
	}
}

func main() {
	// Output file for the SQL script
	outputFile, err := os.Create("generate_stat_upgrades.sql")
	if err != nil {
		fmt.Println("Error creating file:", err)
		return
	}
	defer outputFile.Close()

	// Stats names for clarity
	stats := map[int]string{
		STAT_INTELLECT: "Intellect",
		STAT_SPIRIT:    "Spirit",
		STAT_STRENGTH:  "Strength",
		STAT_AGILITY:   "Agility",
		STAT_STAMINA:   "Stamina",
	}

	resistTypes := map[int]string{
		RESIST_ARCANE: "Arcane",
		RESIST_FIRE:   "Fire",
		RESIST_NATURE: "Nature",
		RESIST_FROST:  "Frost",
		RESIST_SHADOW: "Shadow",
	}

	// Start writing the SQL script
	fmt.Fprintln(outputFile, "-- SQL Script to Insert 50 Ranks for Each Stat")
	fmt.Fprintln(outputFile, "INSERT INTO mp_upgrade_ranks (upgradeRank, advancementId, itemEntry1, itemCost1, itemEntry2, itemCost2, itemEntry3, itemCost3, minIncrease1, maxIncrease1, minIncrease2, maxIncrease2, minIncrease3, maxIncrease3, chanceCost1, chanceCost2, chanceCost3) VALUES")

	// Iterate over stats
	for statID := range stats {
		for rank := 1; rank <= 50; rank++ {
			// Material cost increases by 50 per rank
			materialCost := 50
			if rank < 11 {
				materialCost = 100 + (rank-1)*50
			}
			if rank >= 11 && rank < 30 {
				materialCost = 500 + (rank-11)*25
			}
			if rank >= 30 {
				materialCost = 1000 + (rank-30)*18
			}

			// Make adjustment for new fusion core types
			itemCost1 := int(math.Ceil(float64(materialCost) / 20))

			// Stat growth
			minIncrease1 := 1 + (rank-1)/10*2
			maxIncrease1 := 10 + (rank-1)/10*2

			minIncrease2 := (minIncrease1 + maxIncrease1) / 2
			maxIncrease3Bonus := (rank-1)/10*2 + 1
			maxIncrease3 := maxIncrease1 + maxIncrease3Bonus
			minIncrease3 := maxIncrease3 - 3

			// Dice costs
			chanceCost1 := 20 + (rank-1)*3
			chanceCost2 := 50 + (rank-1)*3
			chanceCost3 := 75 + (rank-1)*3

			// use material ids from the mp_material_types table material1 should be common stuff.
			itemEntry1 := getStatItemEntry(statID, 3)

			// material2 should be rare stuff only required after rank 10 at growth rate of 5 per rank
			itemEntry2, itemCost2 := 0, 0
			if rank > 10 {
				itemEntry2 = getStatItemEntry(statID, 4)
				itemCost2 = (rank - 11) * 10
			}

			// Adjust from old formula to new mythic fusion core types
			itemCost2 = int(math.Ceil(float64(itemCost2) / 5))

			itemEntry3, itemCost3 := 0, 0
			if rank >= 30 {
				itemEntry3 = 911002 // veilstones
				itemCost3 = (rank - 29) * 3
				if itemCost3 > 15 {
					itemCost3 = 15
				}
			}

			// Write SQL insert statement for this rank
			sql := fmt.Sprintf(
				"(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				rank, statID, itemEntry1, itemCost1, itemEntry2, itemCost2, itemEntry3, itemCost3,
				minIncrease1, maxIncrease1, minIncrease2, maxIncrease1, minIncrease3, maxIncrease3,
				chanceCost1, chanceCost2, chanceCost3,
			)

			// Add a comma for all but the last line
			isLastStat := statID == STAT_STAMINA && rank == 50
			isLastResist := false
			isLast := isLastStat && len(resistTypes) == 0 || isLastResist

			if !isLast {
				sql += ","
			}
			fmt.Fprintln(outputFile, sql)
		}
	}

	// Iterate over resists
	for resistId := range resistTypes {
		resistIdbump := resistId + 5
		for rank := 1; rank <= 50; rank++ {
			// Material cost increases by 50 per rank
			materialCost := 50
			if rank < 11 {
				materialCost = 75 + (rank-1)*35
			}
			if rank >= 11 && rank < 30 {
				materialCost = 350 + (rank-11)*25
			}
			if rank >= 30 {
				materialCost = 700 + (rank-30)*18
			}

			// Make adjustment for new fusion core types
			itemCost1 := int(math.Ceil(float64(materialCost) / 20))

			// Stat growth
			minIncrease1 := 1 + (rank-1)/5*2
			maxIncrease1 := 5 + (rank-1)/5*2

			minIncrease2 := (minIncrease1 + maxIncrease1) / 2
			maxIncrease3Bonus := (rank-1)/5*2 + 1

			maxIncrease3 := maxIncrease1 + maxIncrease3Bonus
			minIncrease3 := maxIncrease3 - 2

			// Dice costs
			chanceCost1 := 20 + (rank-1)*3
			chanceCost2 := 50 + (rank-1)*3
			chanceCost3 := 75 + (rank-1)*3

			// use material ids from the mp_material_types table material1 should be common stuff.
			itemEntry1 := getResistItemEntry(resistId, 3)

			// material2 should be rare stuff only required after rank 10 at growth rate of 5 per rank
			itemEntry2, itemCost2 := 0, 0
			if rank > 10 {
				itemEntry2 = getResistItemEntry(resistId, 4)
				itemCost2 = (rank - 11) * 10
			}

			// Adjust from old formula to new mythic fusion core types
			itemCost2 = int(math.Ceil(float64(itemCost2) / 5))

			itemEntry3, itemCost3 := 0, 0
			if rank >= 30 {
				itemEntry3 = 911002 // veilstones
				itemCost3 = (rank - 29) * 3
			}

			// Write SQL insert statement for this rank
			sql := fmt.Sprintf(
				"(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				rank, resistIdbump, itemEntry1, itemCost1, itemEntry2, itemCost2, itemEntry3, itemCost3,
				minIncrease1, maxIncrease1, minIncrease2, maxIncrease1, minIncrease3, maxIncrease3,
				chanceCost1, chanceCost2, chanceCost3,
			)

			// Add a comma for all but the last line
			isLast := resistId == RESIST_ARCANE && rank == 50

			if !isLast {
				sql += ","
			}
			fmt.Fprintln(outputFile, sql)
		}
	}

	fmt.Fprintln(outputFile, ";")
	fmt.Println("SQL script generated: generate_stat_upgrades.sql")
}
