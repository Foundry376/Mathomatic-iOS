//
//  EquationTableViewCell.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationTableViewCell.h"


@implementation EquationTableViewCell

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier {
    if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {

    }
    return self;
}

- (void)setCommand:(MathCommand *)c
{
    [self setEquationText: @""];
    
    [command release];
    command = [c retain];
    [command attachToView: self];
}

- (void)setEquation:(EquationView *)e
{
    [self setEquationText: @""];
 
    [e setFrame: CGRectMake(0, 0, 320, [e equationHeight])];
    [self addSubview: e];
}

- (void)setEquationText:(NSString*)t
{
    [[self subviews] makeObjectsPerformSelector: @selector(removeFromSuperview)];
    self.text = t;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {

    [super setSelected:selected animated:animated];
}


- (void)dealloc {
    [command release];
    [super dealloc];
}


@end
