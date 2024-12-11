package main

import (
	"fmt"
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
	RESIST_FROST = iota
	RESIST_FIRE
	RESIST_NATURE
	RESIST_SHADOW
	RESIST_ARCANE
)

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

	// Start writing the SQL script
	fmt.Fprintln(outputFile, "-- SQL Script to Insert 50 Ranks for Each Stat")
	fmt.Fprintln(outputFile, "INSERT INTO mp_upgrade_ranks (upgradeRank, advancementId, materialId1, materialCost1, materialId2, materialCost2, materialId3, materialCost3, minIncrease1, maxIncrease1, minIncrease2, maxIncrease2, minIncrease3, maxIncrease3, chanceCost1, chanceCost2, chanceCost3) VALUES")

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
			materialId1 := statID*2 + 1

			// material2 should be rare stuff only required after rank 10 at growth rate of 5 per rank
			materialId2, materialCost2 := 0, 0
			if rank > 10 {
				materialId2 = statID*2 + 2
				materialCost2 = (rank - 11) * 10
			}

			materialId3, materialCost3 := 0, 0
			if rank >= 30 {
				materialId3 = 20 // Group lot of raid only items
				materialCost3 = (rank - 29) * 3
			}

			// Write SQL insert statement for this rank
			sql := fmt.Sprintf(
				"(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				rank, statID, materialId1, materialCost, materialId2, materialCost2, materialId3, materialCost3,
				minIncrease1, maxIncrease1, minIncrease2, maxIncrease1, minIncrease3, maxIncrease3,
				chanceCost1, chanceCost2, chanceCost3,
			)

			// Add a comma for all but the last line
			if !(statID == STAT_SPIRIT && rank == 50) {
				sql += ","
			}
			fmt.Fprintln(outputFile, sql)
		}
	}

	fmt.Fprintln(outputFile, ";")
	fmt.Println("SQL script generated: generate_stat_upgrades.sql")
}
