#include "headers.hpp"

void rights::newClient(std::string name, t_right rightList) {
	vecteurMap[name] = rightList;
}

void rights::setWrite(std::string name, bool newValue) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		vecteurMap[name].write = newValue;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
	}
}

void rights::setQuick(std::string name, bool newValue) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		vecteurMap[name].quick = newValue;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
	}
}

void rights::setInvit(std::string name, bool newValue) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		vecteurMap[name].invit = newValue;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
	}
}

void rights::setGive(std::string name, bool newValue) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		vecteurMap[name].give = newValue;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
	}
}

void rights::setModes(std::string name, bool newValue) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		vecteurMap[name].modes = newValue;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
	}
}

bool rights::getWrite(std::string name) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		return vecteurMap[name].write;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
		// Retourner une valeur par défaut (par exemple, false)
		return false;
	}
}

bool rights::getQuick(std::string name) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		return vecteurMap[name].quick;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
		// Retourner une valeur par défaut (par exemple, false)
		return false;
	}
}

bool rights::getInvit(std::string name) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		return vecteurMap[name].invit;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
		// Retourner une valeur par défaut (par exemple, false)
		return false;
	}
}

bool rights::getGive(std::string name) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		return vecteurMap[name].give;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
		// Retourner une valeur par défaut (par exemple, false)
		return false;
	}
}

bool rights::getModes(std::string name) {
	if (vecteurMap.find(name) != vecteurMap.end()) {
		return vecteurMap[name].modes;
	} else {
		std::cerr << "The client with the name " << name << " does not exist." << std::endl;
		// Retourner une valeur par défaut (par exemple, false)
		return false;
	}
}

bool rights::isAllPower(std::string name) {
    int nb_rights = 0;

	if (vecteurMap.find(name) != vecteurMap.end()) {
	nb_rights += vecteurMap[name].write;
    nb_rights += vecteurMap[name].quick;
    nb_rights += vecteurMap[name].invit;
    nb_rights += vecteurMap[name].modes;
    nb_rights += vecteurMap[name].give;
    }
    if (nb_rights == 5)
        return true;
    return false;
}


t_right	rights::BasicRights() {
	t_right right;

	right.write = 1;
	right.quick = 0;
	right.invit = 0;
	right.modes = 0;
	right.give = 0;
	return right;
}

t_right	rights::RootRights() {
	t_right right;

	right.write = 1;
	right.quick = 1;
	right.invit = 1;
	right.modes = 1;
	right.give = 1;
	return right;
}

void rights::changeClientName(std::string oldName, std::string newName) {
	std::map<std::string, t_right>::iterator it = vecteurMap.find(oldName);
	if (it != vecteurMap.end()) {
		vecteurMap[newName] = it->second; // ajoute avec le nouveau nom
		vecteurMap.erase(it); // supprime l'ancien nom
	}
}

std::string rights::getTrueRights(std::string name) {
	std::map<std::string, t_right>::iterator it = vecteurMap.find(name);
	if (it != vecteurMap.end()) {
		t_right rights = it->second;
		std::stringstream ss;
		const char* sep = "";

		struct Right {
			const char* name;
			bool t_right::*value;
		};

		Right rightsArray[] = {
			{"write", &t_right::write},
			{"quick", &t_right::quick},
			{"invit", &t_right::invit},
			{"modes", &t_right::modes},
			{"give", &t_right::give}
		};

		for (int i = 0; i < 5; ++i) {
			if (rights.*(rightsArray[i].value)) {
				ss << sep << rightsArray[i].name;
				sep = ", ";
			}
		}

		return ss.str();
	}
	return "";
}

void rights::changeRight(std::string c, std::string cTarget, std::string right, bool x) {
	// Vérifie si le client cible existe dans la liste de droits
	if (vecteurMap.find(c) == vecteurMap.end()) {
		// Si non, affiche un message d'erreur et quitte la fonction
		std::cout << "Error: Client " << c << " not found." << std::endl;
		return;
	}
	// Vérifie si le client c a le droit de modifier les droits
	if (!vecteurMap[c].give) {
		// Si non, affiche un message d'erreur et quitte la fonction
		std::cout << "Error: " << c << " does not have permission to change rights." << std::endl;
		return;
	}
	// Vérifie si le client cible existe dans la liste de droits
	if (vecteurMap.find(cTarget) == vecteurMap.end()) {
		// Si non, affiche un message d'erreur et quitte la fonction
		std::cout << "Error: Client " << cTarget << " not found." << std::endl;
		return;
	}
	// Détermine quel droit doit être modifié
	if (right == "write") {
		vecteurMap[cTarget].write = x;
	} else if (right == "quick") {
		vecteurMap[cTarget].quick = x;
	} else if (right == "invit") {
		vecteurMap[cTarget].invit = x;
	} else if (right == "modes") {
		vecteurMap[cTarget].modes = x;
	} else if (right == "give") {
		vecteurMap[cTarget].give = x;
	} else {
		// Si le droit spécifié n'est pas valide, affiche un message d'erreur et quitte la fonction
		std::cout << "Error: Invalid right specified." << std::endl;
		return;
	}
	// Affiche un message de confirmation
	std::cout << "Rights changed successfully for client " << cTarget << std::endl;
}

void	rights::setRights(std::string name, t_right newRights)
{
	vecteurMap[name] = newRights;
}