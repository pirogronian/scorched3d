////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLMENUENTRY_H__5AE3319D_D83E_4392_BABB_7E34B2D69BD8__INCLUDED_)
#define AFX_GLMENUENTRY_H__5AE3319D_D83E_4392_BABB_7E34B2D69BD8__INCLUDED_

#include <GLEXT/GLMenu.h>
#include <GLW/GLWSelector.h>

class GLTexture;
class Image;
class GLMenuEntry : public GLWSelectorI
{
public:
	GLMenuEntry(char *menuName, 
		float width, 
		unsigned int state,
		GLMenuI *callback,
		Image *icon,
		unsigned int flags);
	virtual ~GLMenuEntry();

	bool click(float currentTop, int x, int y);
	unsigned int getState() { return state_; }
	void draw(float currentTop, float currentLeft);

	void addMenuItem(GLMenuItem &item);
	float getX() { return left_; }
	float getY() { return top_; }
	float getW() { return width_; }
	float getH() { return height_; }
	bool getSelected() { return selected_; }
	const char *getName() { return menuName_.c_str(); }
	GLMenuI *getCallback() { return callback_; }
	unsigned int getFlags() { return flags_; }

	virtual void itemSelected(GLWSelectorEntry *entry, int position);
	virtual void noItemSelected();

protected:
	bool selected_;
	float left_, top_;
	float width_, height_;
	unsigned int state_;
	unsigned int flags_;
	GLMenuI *callback_;
	GLTexture *texture_;
	Image *icon_;
	std::list<GLMenuItem> menuItems_;
	std::string menuName_;

	void drawText();
	void drawIcon();

};

#endif // !defined(AFX_GLMENUENTRY_H__5AE3319D_D83E_4392_BABB_7E34B2D69BD8__INCLUDED_)
