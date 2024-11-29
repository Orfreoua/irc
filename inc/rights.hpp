#ifndef RIGHTS_HPP
#define RIGHTS_HPP

#include "headers.hpp"

//structure de boolen representant les droits d'un utilisateur.
typedef struct {
	bool write;	// permet decrire sur un chan 
	bool quick;	// permet de suprimer une personne du chan
	bool invit;	// permet d'inviter une personne dans le chan
	bool modes;	//permet de modifier les settings du chan 
	bool give;	// permet de changer les droits d'un utilisateur.
} t_right;

class rights {
private:
//liste avec les utilisateurs plus leurs droits.
	std::map<std::string, t_right> vecteurMap;

public:
//ajoute un clients dans la liste de droits.
	void newClient(std::string name, t_right rightList);

//setter de droits
	void setWrite(std::string name, bool newValue);
	void setQuick(std::string name, bool newValue);
	void setInvit(std::string name, bool newValue);
	void setGive(std::string name, bool newValue);
	void setModes(std::string name, bool newValue);

//getter de droits
	bool getWrite(std::string name);
	bool getQuick(std::string name);
	bool getInvit(std::string name);
	bool getGive(std::string name);
	bool getModes(std::string name);

//renvoie une struc avec des droit pour un utilisateur basic ou root.
	t_right		BasicRights();
	t_right		RootRights();

//change le nom dans la liste (devra etre utilisé dans nick si on prend nick pour reference).
    void changeClientName(std::string oldName, std::string newName);

//imprime les droits que le client posede
std::string getTrueRights(std::string name);

//change les droits d'un utilisateur: prend en param le client, le client cliblé, le droit ciblé et le booléen du droit à changer.
void	changeRight(std::string c, std::string cTarget, std::string right, bool x);
bool	isAllPower(std::string name);
void	setRights(std::string name, t_right newRights);
};

#endif