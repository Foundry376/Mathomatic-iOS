//
//  EquationTableViewCell.h
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MathCommand.h"

@interface EquationTableViewCell : UITableViewCell {

    MathCommand * command;
}

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier;
- (void)setCommand:(MathCommand *)c;
- (void)setEquation:(EquationView *)e;
- (void)setSelected:(BOOL)selected animated:(BOOL)animated;
- (void)dealloc;

@end
