#pragma once
#include <QtCore>

namespace defs {

	enum class EBanks {
		eGLS,
		eDKB,
		eUndefined
	};

	inline EBanks bankFromString(const QString& strBank) {
		if (strBank == "GLS") return EBanks::eGLS;
		if (strBank == "DKB") return EBanks::eDKB;
		return EBanks::eUndefined;
	}

	inline const int cNoFieldGLS = 19; // Number of columns
	inline const int cIngoreLinesGLS = 1; // Number of initial lines to ignore
	inline QMap<QString, int> mpCellsGLS = {
		{"Bezeichnung", 0},
		{"IBAN Auftragskonto", 1},
		{"BIC Auftragskonto", 2},
		{"Bankname Auftragskonto", 3},
		{"Buchungstag", 4},
		{"Valutadatum", 5},
		{"Name Zahlungsbeteiligter", 6},
		{"IBAN Zahlungsbeteiligter", 7},
		{"BIC (SWIFT-Code) Zahlungsbeteiligter", 8},
		{"Buchungstext", 9},
		{"Verwendungszweck", 10},
		{"Betrag", 11},
		{"Waehrung", 12},
		{"Saldo nach Buchung", 13},
		{"Bemerkung", 14},
		{"Kategorie", 15},
		{"Steuerrelevant", 16},
		{"Glaeubiger ID", 17},
		{"Mandatsreferenz", 18}
	};

}