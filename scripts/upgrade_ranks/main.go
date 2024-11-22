package main

import (
	"fmt"
	"os"
)

// Stats enum
const (
	STAT_STRENGTH = iota
	STAT_AGILITY
	STAT_STAMINA
	STAT_INTELLECT
	STAT_SPIRIT
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
		STAT_STRENGTH:  "STAT_STRENGTH",
		STAT_AGILITY:   "STAT_AGILITY",
		STAT_STAMINA:   "STAT_STAMINA",
		STAT_INTELLECT: "STAT_INTELLECT",
		STAT_SPIRIT:    "STAT_SPIRIT",
	}

	// Start writing the SQL script
	fmt.Fprintln(outputFile, "-- SQL Script to Insert 50 Ranks for Each Stat")
	fmt.Fprintln(outputFile, "INSERT INTO mp_stat_upgrade_ranks (upgradeRank, statTypeId, materialId1, materialCost, materialId2, materialCost2, materialId3, materialCost3, minIncrease1, maxIncrease1, minIncrease2, maxIncrease2, minIncrease3, maxIncrease3, chanceCost1, chanceCost2, chanceCost3) VALUES")

	// Iterate over stats
	for statID := range stats {
		for rank := 1; rank <= 50; rank++ {
			// Material cost increases by 50 per rank
			materialCost := 100 + (rank-1)*50

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

			// Write SQL insert statement for this rank
			sql := fmt.Sprintf(
				"(%d, %d, 1001, %d, 0, 0, 0, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
				rank, statID, materialCost,
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
